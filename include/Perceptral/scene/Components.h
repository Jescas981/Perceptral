#pragma once
#include <Eigen/Eigen>
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Macros.h>
#include <Perceptral/renderer/Shader.h>
#include <Perceptral/renderer/Material.h>
#include <Perceptral/renderer/VertexArray.h>

namespace Perceptral {
class Scriptable;

namespace Component {

/***************************
    CAMERA FUNCTIONALITY
***************************/

struct PC_API Camera {
  float fov = 45.0f;
  float aspectRatio = 16.0f / 9.0f;
  float nearPlane = 0.1f;
  float farPlane = 1000.0f;

  bool autoAspect{true};

  Eigen::Matrix4f projectionMatrix = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f viewMatrix = Eigen::Matrix4f::Identity();
  Eigen::Matrix4f viewProjectionMatrix = Eigen::Matrix4f::Identity();

  bool needsProjectionUpdate = true;
  bool needsViewUpdate = true;
};

struct PC_API MainCamera {};

struct PC_API ActiveCamera {};

struct OrbitCameraController {
  Eigen::Vector3f target{0, 0, 0};

  float distance = 10.0f;
  float zoomSpeed = 1.0f;
  float rotationSpeed = 0.05f;
  float yaw = 0.0f;
  float pitch = 0.0f;
  float minDistance = 0.5f;
  float maxDistance = 1000.0f;
  float minPitch = -89.0f;
  float maxPitch = 89.0f;

  bool firstMovement = false;
  bool isRotating = false;
  float lastMouseX = 0.0f;
  float lastMouseY = 0.0f;
};

/***************************
    SCRIPT FUNCTIONALITY
***************************/

struct NativeScript {
  bool enabled{true};
  Scriptable *instance = nullptr;

  Scriptable *(*instantiate)();
  void (*destroy)(NativeScript *);
};

/***************************
  RENDERABLE FUNCTIONALITY
***************************/

struct PC_API MeshData {
  std::vector<Eigen::Vector3f> vertices;
  std::vector<uint32_t> indices;
  bool isDirty{true};

  MeshData() = default;
  MeshData(const MeshData &other) = default;
  MeshData(MeshData &&other) noexcept = default;

  MeshData(std::vector<Eigen::Vector3f> vertices, std::vector<uint32_t> indices)
      : vertices(std::move(vertices)), indices(std::move(indices)) {}
};

struct PC_API MeshRenderer {
  Material material;
  bool visible{true};

  struct GPU {
    std::shared_ptr<VertexArray> vao{nullptr};
    std::size_t indexCount = 0;
  } gpu;

  MeshRenderer() = default;
  MeshRenderer(Material material) : material(std::move(material)) {}
};

/***************************
    COMMON FUNCTIONALITY
***************************/

struct PC_API Tag {
  std::string tag;
};

struct PC_API Transform {
  Eigen::Vector3f translation = Eigen::Vector3f::Zero();
  Eigen::Quaternionf rotation = Eigen::Quaternionf::Identity();
  Eigen::Vector3f scale = Eigen::Vector3f::Ones();
};

} // namespace Component
} // namespace Perceptral