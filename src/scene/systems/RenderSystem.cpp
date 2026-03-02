#include "Perceptral/scene/Components.h"
#include "Perceptral/scene/Scene.h"
#include <Perceptral/core/math/TransformUtils.h>
#include <Perceptral/renderer/Renderer.h>
#include <Perceptral/scene/systems/RenderSystem.h>
#include <iostream>

namespace Perceptral {

void RenderSystem::onRender(Scene &scene) {
  auto &registry = scene.getRegistry();

  auto camView = registry.view<Component::Camera, Component::MainCamera>();
  if (camView.begin() == camView.end()) {
    return;
  }

  auto cameraEntity = *camView.begin();
  auto &camera = camView.get<Component::Camera>(cameraEntity);

  renderMeshes(registry, camera);
  renderPointClouds(registry, camera);
}

void RenderSystem::renderMeshes(entt::registry &registry,
                                const Component::Camera &camera) {
  auto view = registry.view<Component::Transform, Component::MeshRenderer,
                            Component::MeshData>();

  for (auto entity : view) {
    auto &transform = view.get<Component::Transform>(entity);
    auto &renderer = view.get<Component::MeshRenderer>(entity);
    auto &meshData = view.get<Component::MeshData>(entity);

    // Skip if not visible or invalid
    if (!renderer.visible) {
      continue;
    }

    // Bind shader
    auto &material = renderer.material;
    Eigen::Matrix4f modelMatrix = Math::toMatrix(transform);
    auto shader = material.getShader();

    // Check if shader exists
    if (!shader) {
      continue; // Skip this entity if no shader
    }

    shader->bind();
    // Load uniforms
    shader->setMat4("u_ProjectionView", camera.viewProjectionMatrix);
    shader->setMat4("u_Model", modelMatrix);
    shader->setFloat3("u_Color", material.getColor());
    shader->setFloat("u_Opacity", material.getOpacity());

    if (meshData.isDirty) {
      // Create VAO if it doesn't exist yet
      if (!renderer.gpu.vao) {
        renderer.gpu.vao = VertexArray::create();
      }

      // Upload to GPU
      auto vertexBuffer = VertexBuffer::create(meshData.vertices.data(),
                                               meshData.vertices.size() *
                                                   sizeof(Eigen::Vector3f));

      vertexBuffer->setLayout({{ShaderDataType::Float3, "aPosition"}});

      // Attach buffer to VAO
      renderer.gpu.vao->clearVertexBuffers();
      renderer.gpu.vao->addVertexBuffer(vertexBuffer);

      auto indexBuffer =
          IndexBuffer::create(meshData.indices.data(), meshData.indices.size());
      renderer.gpu.vao->setIndexBuffer(indexBuffer);

      renderer.gpu.indexCount = meshData.indices.size();
      meshData.isDirty = false;
    }

    Renderer::drawIndexed(renderer.gpu.vao, renderer.gpu.indexCount);
  }
}

void RenderSystem::renderPointClouds(entt::registry &registry,
                                     const Component::Camera &camera) {
  auto view = registry.view<Component::Transform, Component::PointCloudRenderer,
                            Component::PointCloudData>();

  for (auto entity : view) {
    auto &transform = view.get<Component::Transform>(entity);
    auto &renderer = view.get<Component::PointCloudRenderer>(entity);
    auto &cloudData = view.get<Component::PointCloudData>(entity);

    if (!renderer.visible)
      continue;

    auto shader = renderer.material.getShader();
    if (!shader)
      continue;

    const auto &asset = cloudData.asset;
    if (!asset)
      continue;

    Eigen::Matrix4f modelMatrix = Math::toMatrix(transform);

    shader->bind();
    shader->setMat4("u_ProjectionView", camera.viewProjectionMatrix);
    shader->setMat4("u_Model", modelMatrix);
    shader->setFloat("u_Opacity", renderer.material.getOpacity());
    shader->setFloat("u_PointSize", cloudData.pointSize);
    shader->setFloat3("u_SelectionColor", cloudData.selectionColor);
    shader->setInt("u_ColorMode", static_cast<int>(cloudData.colorMode));

    if (cloudData.colorMode == Component::ColorMode::ScalarField &&
        asset->hasField(cloudData.activeField)) {
      shader->setFloat("uMin", cloudData.fieldMin);
      shader->setFloat("uMax", cloudData.fieldMax);
    } else if (cloudData.colorMode == Component::ColorMode::AxisColorX ||
               cloudData.colorMode == Component::ColorMode::AxisColorY ||
               cloudData.colorMode == Component::ColorMode::AxisColorZ) {
      shader->setFloat("uMin", cloudData.fieldMin);
      shader->setFloat("uMax", cloudData.fieldMax);
    }

    if (cloudData.colorMode == Component::ColorMode::LabelField &&
        asset->hasLabels()) {
      shader->setInt("uLabelCount", static_cast<int>(asset->labelCount()));
    }

    if (cloudData.isDirty) {
      const std::size_t pointCount = asset->size();

      if (!renderer.gpu.vao)
        renderer.gpu.vao = VertexArray::create();
      renderer.gpu.vao->clearVertexBuffers();

      // Location 0 — positions (always)
      std::vector<float> xyz;
      xyz.reserve(pointCount * 3);
      for (std::size_t i = 0; i < pointCount; ++i) {
        xyz.push_back(asset->points[i].x());
        xyz.push_back(asset->points[i].y());
        xyz.push_back(asset->points[i].z());
      }
      auto xyzBuf =
          VertexBuffer::create(xyz.data(), xyz.size() * sizeof(float));
      xyzBuf->setLayout({{ShaderDataType::Float3, "aPosition"}});
      renderer.gpu.vao->addVertexBuffer(xyzBuf);

      // Location 1 — scalar (always uploaded to keep location stable; zeros if
      // unused)
      std::vector<float> scalar(pointCount, 0.0f);

      if (cloudData.autoRange) {
        scalar.resize(pointCount);

        // Lambda to compute min/max and set shader uniforms
        auto computeRange = [&](auto getValue) {
          if (pointCount == 0) {
            cloudData.fieldMin = 0.0f;
            cloudData.fieldMax = 1.0f;
            shader->setFloat("uMin", cloudData.fieldMin);
            shader->setFloat("uMax", cloudData.fieldMax);
            return;
          }

          float mn = std::numeric_limits<float>::max();
          float mx = std::numeric_limits<float>::lowest();

          for (std::size_t i = 0; i < pointCount; ++i) {
            float v = getValue(i);
            mn = std::min(mn, v);
            mx = std::max(mx, v);
          }

          cloudData.fieldMin = mn;
          cloudData.fieldMax = (mx > mn) ? mx : mn + 1.0f; // avoid zero-range
          shader->setFloat("uMin", cloudData.fieldMin);
          shader->setFloat("uMax", cloudData.fieldMax);
        };

        // Lambda to fill scalar from axis
        auto fillAxis = [&](auto accessor) {
          for (std::size_t i = 0; i < pointCount; ++i)
            scalar[i] = accessor(asset->points[i]);
          computeRange([&](std::size_t i) { return scalar[i]; });
        };

        // Fill scalar depending on color mode
        switch (cloudData.colorMode) {
        case Component::ColorMode::AxisColorX:
          fillAxis([](const auto &p) { return p.x(); });
          break;
        case Component::ColorMode::AxisColorY:
          fillAxis([](const auto &p) { return p.y(); });
          break;
        case Component::ColorMode::AxisColorZ:
          fillAxis([](const auto &p) { return p.z(); });
          break;
        case Component::ColorMode::LabelField:
          if (asset->hasLabels())
            scalar.assign(asset->labels.begin(), asset->labels.end());
          break;
        case Component::ColorMode::ScalarField:
          if (asset->hasField(cloudData.activeField)) {
            scalar = asset->fields.at(cloudData.activeField);
            computeRange([&](std::size_t i) { return scalar[i]; });
          }
          break;
        default:
          break;
        }

        // If using labels, also set shader uniform
        if (cloudData.colorMode == Component::ColorMode::ScalarField &&
            asset->hasLabels())
          shader->setInt("uLabelCount", static_cast<int>(asset->labelCount()));
      }

      auto scalarBuf =
          VertexBuffer::create(scalar.data(), scalar.size() * sizeof(float));
      scalarBuf->setLayout({{ShaderDataType::Float, "aScalar"}});
      renderer.gpu.vao->addVertexBuffer(scalarBuf);

      // Location 2 — selection (always uploaded to keep location stable; zeros
      // if unused)
      std::vector<float> sel(pointCount, 0.0f);
      if (!cloudData.selected.empty()) {
        for (std::size_t i = 0; i < pointCount; ++i)
          sel[i] = cloudData.selected[i] ? 1.0f : 0.0f;
      }
      auto selBuf =
          VertexBuffer::create(sel.data(), sel.size() * sizeof(float));
      selBuf->setLayout({{ShaderDataType::Float, "aSelected"}});
      renderer.gpu.vao->addVertexBuffer(selBuf);

      renderer.gpu.pointCount = pointCount;
      cloudData.isDirty = false;
    }

    Renderer::drawArrays(renderer.gpu.vao, 0, renderer.gpu.pointCount,
                         PrimitiveType::Points);
  }
}
} // namespace Perceptral