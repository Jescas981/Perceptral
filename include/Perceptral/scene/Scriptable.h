#pragma once
#include "Perceptral/scene/Entity.h"
#include "Perceptral/scene/Scene.h"
#include "Perceptral/scene/systems/ScriptSystem.h"
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Event.h>
#include <Perceptral/scene/Components.h>
#include <entt/entt.hpp>

namespace Perceptral {
class Scriptable {
public:
  using Ptr = std::shared_ptr<Scriptable>;

  virtual ~Scriptable() = default;

  virtual void onCreate() {}
  virtual void onDestroy() {}
  virtual void onUpdate(DeltaTime dt) { UNUSED(dt); }
  virtual void onEvent(Event &e) { UNUSED(e); }
  virtual void onImgGuiRender() {}
  virtual void onRender() {}

  inline Component::Transform &getTransform() { return *transform_; }
  inline Scene &getScene() { return *scene_; }

  Entity createChild(const std::string &name = "Entity") {
    return scene_->createEntityChild(name, entity_);
  }

  Entity createChild(Entity parent, const std::string &name = "Entity") {
    return scene_->createEntityChild(name, parent);
  }

  Entity &getEntity() { return entity_; }

  Entity getEntityByTag(const std::string &name) {
    return scene_->findEntityByName(name);
  }

private:
  void attach(Entity entity, Scene &scene) {
    entity_ = entity;
    scene_ = &scene;

    if (!entity_.hasComponent<Component::Transform>()) {
      entity_.addComponent<Component::Transform>();
    }

    transform_ = &entity_.getComponent<Component::Transform>();
  }

private:
  friend class ScriptSystem;
  Entity entity_;
  Scene *scene_;
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

template <typename T> T *GetScript(Component::NativeScript &ns) {
  return dynamic_cast<T *>(ns.instance);
}

} // namespace Perceptral