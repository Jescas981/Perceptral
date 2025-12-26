#include <Perceptral/core/math/TransformUtils.h>
#include <Perceptral/renderer/Renderer.h>
#include <Perceptral/scene/systems/RenderSystem.h>
#include <iostream>

namespace Perceptral {

void RenderSystem::onRender(entt::registry &registry) {
  auto camView = registry.view<Component::Camera, Component::MainCamera>();
  if (camView.begin() == camView.end()) {
    return;
  }

  auto cameraEntity = *camView.begin();
  auto &camera = camView.get<Component::Camera>(cameraEntity);

  renderMeshes(registry, camera);
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

} // namespace Perceptral