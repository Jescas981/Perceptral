#pragma once

#include <Perceptral/scene/systems/System.h>

namespace Perceptral {
class ScriptSystem : public System {
public:
  void onCreate(entt::registry &registry) override;
  void onUpdate(entt::registry &registry, DeltaTime deltaTime) override;
  void onEvent(entt::registry &registry, Event &event) override;
  void onDestroy(entt::registry &registry) override;
};

} // namespace Perceptral