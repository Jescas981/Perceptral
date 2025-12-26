#include "Perceptral/core/Event.h"
#include "Perceptral/core/Log.h"
#include "Perceptral/core/math/TransformUtils.h"
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/CameraSystem.h>
#include <iostream>

namespace Perceptral {

void CameraSystem::onUpdate(entt::registry &registry, DeltaTime deltaTime) {
  UNUSED(deltaTime);

  auto view = registry.view<Component::Transform, Component::Camera>();

  for (auto entity : view) {
    auto &transform = view.get<Component::Transform>(entity);
    auto &camera = view.get<Component::Camera>(entity);

    if (camera.needsProjectionUpdate) {
      camera.projectionMatrix = createPerspectiveMatrix(
          camera.fov, camera.aspectRatio, camera.nearPlane, camera.farPlane);
      camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
      camera.needsProjectionUpdate = false;
    }

    if (camera.needsViewUpdate) {
      Eigen::Vector3f forward = transform.rotation * Eigen::Vector3f::UnitY();
      Eigen::Vector3f target = transform.translation + forward;
      camera.viewMatrix = Math::lookAt(transform.translation, target);
      camera.viewProjectionMatrix = camera.projectionMatrix * camera.viewMatrix;
      camera.needsViewUpdate = false;
    }
  }
}

void CameraSystem::onEvent(entt::registry &registry, Event &event) {
  EventDispatcher dispatcher(event);

  dispatcher.dispatch<WindowResizeEvent>([&registry](WindowResizeEvent &e) {
    float aspect =
        static_cast<float>(e.getWidth()) / static_cast<float>(e.getHeight());

    auto view = registry.view<Component::Camera>();
    for (auto entity : view) {
      auto &camera = view.get<Component::Camera>(entity);
      if (camera.autoAspect) {
        camera.aspectRatio = aspect;
        camera.needsProjectionUpdate = true;
      }
    }

    return false;
  });
}

Eigen::Matrix4f CameraSystem::createPerspectiveMatrix(float fov, float aspect,
                                                      float near, float far) {
  Eigen::Matrix4f perspective = Eigen::Matrix4f::Zero();

  float tanHalfFov = std::tan(fov / 2.0f);
  float f = 1.0f / tanHalfFov;

  perspective(0, 0) = f / aspect;
  perspective(1, 1) = f;
  perspective(2, 2) = -(far + near) / (far - near);
  perspective(2, 3) = -(2.0f * far * near) / (far - near);
  perspective(3, 2) = -1.0f;

  return perspective;
}

} // namespace Perceptral