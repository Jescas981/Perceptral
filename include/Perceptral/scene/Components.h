#pragma once
#include <Eigen/Eigen>
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Macros.h>
#include <Perceptral/core/assets/PointCloud.h>
#include <Perceptral/renderer/Material.h>
#include <Perceptral/renderer/Shader.h>
#include <Perceptral/renderer/VertexArray.h>
#include <entt/entt.hpp>

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
  bool isMoving = false;
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

enum ColorMode {
  FlatColor,
  RGBField,
  LabelField,
  ScalarField,
  AxisColorX,
  AxisColorY,
  AxisColorZ,
};

struct PointCloudData {
  // Asset reference — actual xyz/fields data lives here, shared
  std::shared_ptr<Asset::PointCloud> asset;

  // Rendering intent — what the renderer should use
  std::string activeField;
  ColorMode colorMode = ColorMode::FlatColor;

  // Selection — per-point, lives here since it's per-entity state
  std::vector<bool> selected;
  float fieldMin = 0.0f;
  float fieldMax = 1.0f;
  float pointSize = 1.0f;
  Eigen::Vector3f selectionColor{1.0f, 0.0f, 0.0f};
  std::vector<Eigen::Vector3f>
      labelColors; // Color for each label ID (index = label ID)

  bool isDirty = true;
  bool autoRange = true;

  PointCloudData() = default;
  PointCloudData(const PointCloudData &other) = default;
  PointCloudData(PointCloudData &&other) noexcept = default;
  PointCloudData(Asset::PointCloud::Ptr asset) : asset(asset) {}
};

struct PointCloudRenderer {
  Material material;
  bool visible{true};

  struct GPU {
    std::shared_ptr<VertexArray> vao{nullptr};
    std::size_t pointCount = 0;
  } gpu;

  PointCloudRenderer() = default;
  PointCloudRenderer(Material material) : material(std::move(material)) {}
};

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

struct PC_API Relationship {
  entt::entity parent = entt::null;
  std::vector<entt::entity> children = {};

  Relationship(entt::entity parent) : parent(parent) {}
  Relationship() {}
};

} // namespace Component
} // namespace Perceptral