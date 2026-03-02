#pragma once
#include <Perceptral/scene/Scene.h>
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/System.h>
#include <entt/entt.hpp>

namespace Perceptral {

class CameraControllerSystem : public System {
public:
  void onUpdate(Scene &scene, DeltaTime deltaTime) override;
  void onEvent(Scene &scene, Event &event) override;
};

} // namespace Perceptral
