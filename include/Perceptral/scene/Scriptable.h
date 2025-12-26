#pragma once
#include "Perceptral/scene/Entity.h"
#include "Perceptral/scene/systems/ScriptSystem.h"
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Event.h>
#include <Perceptral/scene/Components.h>
#include <entt/entt.hpp>

namespace Perceptral {
class Scriptable {
public:
  virtual ~Scriptable() = default;

  virtual void onCreate() {}
  virtual void onDestroy() {}
  virtual void onUpdate(DeltaTime dt) { UNUSED(dt); }
  virtual void onEvent(Event &e) { UNUSED(e); }

  inline Component::Transform &getTransform() { return *transform_; }
  Entity &getEntity() { return entity_; }

private:
  void attach(Entity entity) {
    entity_ = entity;

    if (!entity_.hasComponent<Component::Transform>()) {
      entity_.addComponent<Component::Transform>();
    }

    transform_ = &entity_.getComponent<Component::Transform>();
  }

private:
  friend class ScriptSystem;
  Entity entity_;
  Component::Transform *transform_{nullptr};
};

template <typename T> void BindNativeScript(Component::NativeScript &ns) {
  static_assert(std::is_base_of_v<Scriptable, T>,
                "T must derive from Scriptable");

  ns.instantiate = []() -> Scriptable * { return new T(); };

  ns.destroy = [](Component::NativeScript *ns) {
    delete ns->instance;
    ns->instance = nullptr;
  };
}

} // namespace Perceptral