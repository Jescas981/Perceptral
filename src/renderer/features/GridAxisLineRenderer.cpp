#include "Perceptral/renderer/Renderer.h"
#include <Perceptral/core/Log.h>
#include <Perceptral/renderer/Buffer.h>
#include <Perceptral/renderer/Shader.h>
#include <Perceptral/renderer/features/GridAxisLineRenderer.h>

namespace Perceptral {

GridAxisLineRenderer::GridAxisLineRenderer() {}

GridAxisLineRenderer::~GridAxisLineRenderer() {}

void GridAxisLineRenderer::initialize() {
  vertexArray_ = VertexArray::create();
  shader_ = Shader::create("assets/shaders/gridaxislines.glsl");

  struct Vertex {
    float x, y, z;
    float r, g, b, a;
  };

  std::vector<Vertex> vertices;

  // Grid parameters
  const float gridSize = 100.0f; // Total size of grid
  const int gridLines = 100;     // Number of lines in each direction
  const float gridStep = gridSize / gridLines;
  const float halfSize = gridSize / 2.0f;

  // Grid color (subtle gray)
  const float gridR = 0.3f, gridG = 0.3f, gridB = 0.3f;

  // Create grid on XY plane (Z=0, since Z is up)
  // Lines parallel to X axis (varying Y)
  for (int i = 0; i <= gridLines; ++i) {
    float y = -halfSize + i * gridStep;

    // Determine if this is a major grid line (every 10)
    bool isMajor = (i % 10 == 0);
    float alpha = isMajor ? 0.5f : 0.3f;

    // Skip the lines that coincide with axes (we'll draw them separately)
    if (std::abs(y) < 0.01f)
      continue;

    vertices.push_back({-halfSize, y, 0.0f, gridR, gridG, gridB, alpha});
    vertices.push_back({halfSize, y, 0.0f, gridR, gridG, gridB, alpha});
  }

  // Lines parallel to Y axis (varying X)
  for (int i = 0; i <= gridLines; ++i) {
    float x = -halfSize + i * gridStep;

    bool isMajor = (i % 10 == 0);
    float alpha = isMajor ? 0.5f : 0.3f;

    if (std::abs(x) < 0.01f)
      continue;

    vertices.push_back({x, -halfSize, 0.0f, gridR, gridG, gridB, alpha});
    vertices.push_back({x, halfSize, 0.0f, gridR, gridG, gridB, alpha});
  }

  gridVertexCount_ = vertices.size();

  // Now add the main axis lines (thicker, colored)
  const float axisLength = halfSize;

  // X axis (RED) - extends along X
  vertices.push_back({0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f});
  vertices.push_back({axisLength, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f});
  vertices.push_back({0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f});
  vertices.push_back({-axisLength, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f});

  // Y axis (GREEN) - extends along Y
  vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f});
  vertices.push_back({0.0f, axisLength, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f});
  vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f});
  vertices.push_back({0.0f, -axisLength, 0.0f, 0.0f, 0.5f, 0.0f, 1.0f});

  // Z axis (BLUE) - extends along Z (up)
  vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f});
  vertices.push_back({0.0f, 0.0f, axisLength, 0.0f, 0.0f, 1.0f, 1.0f});
  vertices.push_back({0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 1.0f});
  vertices.push_back({0.0f, 0.0f, -axisLength, 0.0f, 0.0f, 0.5f, 1.0f});

  totalVertexCount_ = vertices.size();

  auto vertexBuffer =
      VertexBuffer::create(vertices.data(), vertices.size() * sizeof(Vertex));
  vertexBuffer->setLayout({{ShaderDataType::Float3, "aPosition"},
                           {ShaderDataType::Float4, "aColor"}});

  vertexArray_->addVertexBuffer(vertexBuffer);

  PC_CORE_INFO(
      "Grid and axis renderer initialized: {} grid lines, {} axis lines",
      gridVertexCount_, totalVertexCount_ - gridVertexCount_);
}

void GridAxisLineRenderer::render(const Component::Camera &camera) {
  if (!shader_ || !vertexArray_) {
    return;
  }

  // Configure render state via Renderer abstraction
  Renderer::setBlending(true);
  Renderer::setDepthTest(true);
  Renderer::setDepthMask(true);

  shader_->bind();
  shader_->setMat4("uViewProjection", camera.viewProjectionMatrix);

  // Draw grid lines (thin)
  Renderer::setLineWidth(1.0f);
  Renderer::drawArrays(vertexArray_, 0, gridVertexCount_, PrimitiveType::Lines);

  // Draw axis lines (thicker)
  Renderer::setLineWidth(3.0f);
  Renderer::drawArrays(
      vertexArray_, gridVertexCount_,
      static_cast<uint32_t>(totalVertexCount_ - gridVertexCount_),
      PrimitiveType::Lines);

  shader_->unbind();

  // Optional: restore defaults if you rely on implicit state
  Renderer::setLineWidth(1.0f);
  Renderer::setBlending(false);
}

} // namespace Perceptral