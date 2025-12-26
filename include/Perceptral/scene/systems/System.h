// scene/systems/System.h
#pragma once
#include <Perceptral/core/DeltaTime.h>
#include <entt/entt.hpp>

namespace Perceptral {

class Event;

class System {
public:
  virtual ~System() = default;

  // Lifecycle methods
  virtual void onCreate(entt::registry &registry) { UNUSED(registry); }
  virtual void onUpdate(entt::registry &registry, DeltaTime deltaTime) {
    UNUSED(registry);
    UNUSED(deltaTime);
  }
  virtual void onRender(entt::registry &registry) { UNUSED(registry); }
  virtual void onEvent(entt::registry &registry, Event &e) {
    UNUSED(registry);
    UNUSED(e);
  }
  virtual void onDestroy(entt::registry &registry) { UNUSED(registry); }
  virtual void onImGuiRender(entt::registry &registry) { UNUSED(registry); }

  // Control
  void setEnabled(bool enabled) { enabled_ = enabled; }
  bool isEnabled() const { return enabled_; }

protected:
  bool enabled_ = true;
};

} // namespace Perceptral