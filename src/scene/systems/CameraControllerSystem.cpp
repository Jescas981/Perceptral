#include "Perceptral/scene/systems/CameraControllerSystem.h"
#include "Perceptral/core/Event.h"
#include "Perceptral/core/Input.h"
#include "Perceptral/core/KeyCodes.h"
#include "Perceptral/scene/Scene.h"
#include <Eigen/src/Core/Matrix.h>
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Log.h>
#include <Perceptral/core/math/TransformUtils.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/System.h>
#include <entt/entt.hpp>

namespace Perceptral {

void CameraControllerSystem::onUpdate(Scene &scene, DeltaTime deltaTime) {
  UNUSED(deltaTime);

  auto &registry = scene.getRegistry();
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

void CameraControllerSystem::onEvent(Scene &scene, Event &event) {
  auto &registry = scene.getRegistry();

  auto view = registry.view<Component::Transform, Component::Camera,
                            Component::OrbitCameraController>();

  EventDispatcher dispatcher(event);

  // --------------------------------------------------------
  // Middle Mouse Pressed (start orbit/pan)
  // --------------------------------------------------------
  dispatcher.dispatch<MouseButtonPressedEvent>([&view](
                                                   MouseButtonPressedEvent &e) {
    if (e.getMouseButton() == MouseButton::Middle) {
      for (auto entity : view) {
        auto &controller = view.get<Component::OrbitCameraController>(entity);

        controller.isMoving = true;
        controller.firstMovement = true;
      }
    }
    return false;
  });

  // --------------------------------------------------------
  // Middle Mouse Released
  // --------------------------------------------------------
  dispatcher.dispatch<MouseButtonReleasedEvent>(
      [&view](MouseButtonReleasedEvent &e) {
        if (e.getMouseButton() == MouseButton::Middle) {
          for (auto entity : view) {
            auto &controller =
                view.get<Component::OrbitCameraController>(entity);

            controller.isMoving = false;
          }
        }
        return false;
      });

  // --------------------------------------------------------
  // Mouse Move
  // --------------------------------------------------------
  dispatcher.dispatch<MouseMovedEvent>([&view](MouseMovedEvent &e) {
    for (auto entity : view) {
      auto &controller = view.get<Component::OrbitCameraController>(entity);
      auto &transform = view.get<Component::Transform>(entity);

      if (controller.firstMovement) {
        controller.lastMouseX = e.getX();
        controller.lastMouseY = e.getY();
        controller.firstMovement = false;
        return false;
      }

      if (controller.isMoving) {
        float deltaX = e.getX() - controller.lastMouseX;
        float deltaY = e.getY() - controller.lastMouseY;

        // ------------------------------------------------
        // Shift + MMB → PAN
        // ------------------------------------------------
        if (Input::isKeyPressed(KeyCode::LeftShift)) {
          // Camera basis vectors (Z-up world)
          Eigen::Vector3f right = transform.rotation * Eigen::Vector3f::UnitX();

          Eigen::Vector3f up = transform.rotation * Eigen::Vector3f::UnitZ();

          // Distance-scaled pan speed
          float panSpeed = 0.002f * controller.distance;

          controller.target -= right * deltaX * panSpeed;
          controller.target += up * deltaY * panSpeed;
        }
        // ------------------------------------------------
        // MMB → ORBIT
        // ------------------------------------------------
        else {
          controller.yaw += deltaX * controller.rotationSpeed;
          controller.pitch -= deltaY * controller.rotationSpeed;
        }

        controller.lastMouseX = e.getX();
        controller.lastMouseY = e.getY();
      }
    }

    return false;
  });

  // --------------------------------------------------------
  // Scroll → Zoom
  // --------------------------------------------------------
  dispatcher.dispatch<MouseScrolledEvent>([&view](MouseScrolledEvent &e) {
    for (auto entity : view) {
      auto &controller = view.get<Component::OrbitCameraController>(entity);

      controller.distance -= e.getYOffset() * controller.zoomSpeed;
    }

    return false;
  });
}

} // namespace Perceptral
