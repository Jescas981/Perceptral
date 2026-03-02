// scene/systems/System.h
#pragma once
#include <Perceptral/core/DeltaTime.h>
#include <entt/entt.hpp>

namespace Perceptral {

class Event;
class Scene;

class System {
public:
  virtual ~System() = default;

  // Lifecycle methods
  virtual void onCreate(Scene &scene) { UNUSED(scene); }
  virtual void onUpdate(Scene &scene, DeltaTime deltaTime) {
    UNUSED(scene);
    UNUSED(deltaTime);
  }
  virtual void onRender(Scene &scene) { UNUSED(scene); }
  virtual void onEvent(Scene &scene, Event &e) {
    UNUSED(scene);
    UNUSED(e);
  }
  virtual void onDestroy(Scene &scene) { UNUSED(scene); }
  virtual void onImGuiRender(Scene &scene) { UNUSED(scene); }

  // Control
  void setEnabled(bool enabled) { enabled_ = enabled; }
  bool isEnabled() const { return enabled_; }

protected:
  bool enabled_ = true;
};

} // namespace Perceptral