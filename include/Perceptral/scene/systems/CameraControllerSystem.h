#pragma once
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/System.h>
#include <entt/entt.hpp>

namespace Perceptral {

class CameraControllerSystem : public System {
public:
  void onUpdate(entt::registry &registry, DeltaTime deltaTime) override;
  void onEvent(entt::registry &registry, Event &event) override;
};

} // namespace Perceptral
