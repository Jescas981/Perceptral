#include "Perceptral/scene/Entity.h"
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/Scriptable.h>
#include <Perceptral/scene/systems/ScriptSystem.h>

namespace Perceptral {

void ScriptSystem::onCreate(entt::registry &registry) {
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);

    if (!nsc.instance) {
      nsc.instance = nsc.instantiate();
      nsc.instance->attach(Entity(entity, &registry));
      nsc.instance->onCreate();
    }
  }
}

void ScriptSystem::onUpdate(entt::registry &registry, DeltaTime dt) {
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);

    if (nsc.instance) {
      nsc.instance->onUpdate(dt);
    }
  }
}

void ScriptSystem::onEvent(entt::registry &registry, Event &e) {
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);
    if (nsc.instance) {
      nsc.instance->onEvent(e);
    }
  }
}

void ScriptSystem::onDestroy(entt::registry &registry) {
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);
    if (nsc.instance) {
      nsc.instance->onDestroy();
      nsc.destroy(&nsc);
    }
  }
}
} // namespace Perceptral