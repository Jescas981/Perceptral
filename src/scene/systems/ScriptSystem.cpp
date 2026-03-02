#include "Perceptral/scene/Entity.h"
#include "Perceptral/scene/Scene.h"
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/Scriptable.h>
#include <Perceptral/scene/systems/ScriptSystem.h>

namespace Perceptral {

void ScriptSystem::onCreate(Scene &scene) {
  auto &registry = scene.getRegistry();
  auto view = registry.view<Component::NativeScript>();
  // Create first
  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);

    if (!nsc.instance) {
      nsc.instance = nsc.instantiate();
      nsc.instance->attach(Entity(entity, &registry), scene);
    }
  }
  // On create now
  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);
    if (nsc.instance) {
      nsc.instance->onCreate();
    }
  }
}

void ScriptSystem::onUpdate(Scene &scene, DeltaTime dt) {
  auto &registry = scene.getRegistry();
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);

    if (nsc.instance) {
      nsc.instance->onUpdate(dt);
    }
  }
}

void ScriptSystem::onEvent(Scene &scene, Event &e) {
  auto &registry = scene.getRegistry();
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);
    if (nsc.instance) {
      nsc.instance->onEvent(e);
    }
  }
}

void ScriptSystem::onImGuiRender(Scene &scene) {
  auto &registry = scene.getRegistry();
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);
    if (nsc.instance) {
      nsc.instance->onImgGuiRender();
    }
  }
}

void ScriptSystem::onRender(Scene &scene) {
  auto &registry = scene.getRegistry();
  auto view = registry.view<Component::NativeScript>();

  for (auto entity : view) {
    auto &nsc = view.get<Component::NativeScript>(entity);
    if (nsc.instance) {
      nsc.instance->onRender();
    }
  }
}

void ScriptSystem::onDestroy(Scene &scene) {
  auto &registry = scene.getRegistry();
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