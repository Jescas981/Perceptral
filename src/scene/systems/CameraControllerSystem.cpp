#include "Perceptral/scene/systems/CameraControllerSystem.h"
#include "Perceptral/core/Event.h"
#include "Perceptral/core/KeyCodes.h"
#include <Eigen/src/Core/Matrix.h>
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Log.h>
#include <Perceptral/core/math/TransformUtils.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/System.h>
#include <entt/entt.hpp>

namespace Perceptral {

void CameraControllerSystem::onUpdate(entt::registry &registry,
                                      DeltaTime deltaTime) {
  UNUSED(deltaTime);
  // Find all entities that have Transform + CameraController
  auto view = registry.view<Component::Transform, Component::Camera,
                            Component::OrbitCameraController>();

  for (auto entity : view) {
    auto &transform = view.get<Component::Transform>(entity);
    auto &controller = view.get<Component::OrbitCameraController>(entity);
    auto &camera = view.get<Component::Camera>(entity);

    // Clamp pitch to avoid gimbal lock
    controller.pitch =
        std::clamp(controller.pitch, controller.minPitch, controller.maxPitch);

    // Clamp distance
    controller.distance = std::clamp(
        controller.distance, controller.minDistance, controller.maxDistance);

    // Compute offset between target and camera
    float pitchRad = controller.pitch * M_PI / 180.0f;
    float yawRad = controller.yaw * M_PI / 180.0f;

    Eigen::Vector3f offset;
    offset.x() = controller.distance * std::cos(pitchRad) * std::sin(yawRad);
    offset.y() = controller.distance * std::cos(pitchRad) * std::cos(yawRad);
    offset.z() = controller.distance * std::sin(pitchRad);

    // Compute position again
    transform.translation = controller.target + offset;

    auto mat = Math::lookRotation((controller.target - transform.translation),
                                  Eigen::Vector3f::UnitZ());

    transform.rotation = Eigen::Quaternionf(mat);

    camera.needsViewUpdate = true;
  }
}

void CameraControllerSystem::onEvent(entt::registry &registry, Event &event) {
  // Find all entities that have Transform + CameraController
  auto view = registry.view<Component::Transform, Component::Camera,
                            Component::OrbitCameraController>();

  auto dispatcher = EventDispatcher(event);

  dispatcher.dispatch<MouseButtonPressedEvent>(
      [&view](MouseButtonPressedEvent &e) {
        for (auto entity : view) {
          auto &controller = view.get<Component::OrbitCameraController>(entity);

          if (e.getMouseButton() == MouseButton::Left) {
            // Rotate here
            controller.isRotating = true;
            controller.firstMovement = true;
          }
        }
        return false;
      });

  dispatcher.dispatch<MouseButtonReleasedEvent>(
      [&view](MouseButtonReleasedEvent &e) {
        for (auto entity : view) {
          auto &controller = view.get<Component::OrbitCameraController>(entity);

          if (e.getMouseButton() == MouseButton::Left) {
            // Rotate here
            controller.isRotating = false;
          }
        }
        return false;
      });

  dispatcher.dispatch<MouseMovedEvent>([&view](MouseMovedEvent &e) {
    for (auto entity : view) {
      auto &controller = view.get<Component::OrbitCameraController>(entity);

      if (controller.firstMovement) {
        controller.lastMouseX = e.getX();
        controller.lastMouseY = e.getY();
        controller.firstMovement = false;
        return false;
      }

      if (controller.isRotating) {
        float deltaX = e.getX() - controller.lastMouseX;
        float deltaY = e.getY() - controller.lastMouseY;

        controller.yaw += deltaX * controller.rotationSpeed;
        controller.pitch += deltaY * controller.rotationSpeed;

        controller.lastMouseX = e.getX();
        controller.lastMouseY = e.getY();
      }
    }
    return false;
  });

  dispatcher.dispatch<MouseScrolledEvent>([&view](MouseScrolledEvent &e) {
    for (auto entity : view) {
      auto &controller = view.get<Component::OrbitCameraController>(entity);
      controller.distance -= e.getYOffset() * controller.zoomSpeed;
    }
    return false;
  });
}

} // namespace Perceptral
