#pragma once
#include "Perceptral/scene/Scene.h"
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/System.h>
#include <entt/entt.hpp>

namespace Perceptral {

class CameraSystem : public System {
public:
  void onUpdate(Scene &scene, DeltaTime deltaTime) override;
  void onEvent(Scene &scene, Event &event) override;

private:
  Eigen::Matrix4f createPerspectiveMatrix(float fov, float aspect, float near,
                                          float far);
};

} // namespace Perceptral
