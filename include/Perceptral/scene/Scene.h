#pragma once

#include "Perceptral/core/Event.h"
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/scene/Entity.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/System.h>
#include <entt/entt.hpp>
#include <string>

namespace Perceptral {

// Scene using EnTT ECS
class PC_API Scene {
public:
  Scene(const std::string &name = "Scene");
  ~Scene();

  // Scene lifecycle
  void onCreate();
  void onUpdate(DeltaTime deltaTime);
  void onRender();
  void onImGuiRender();
  void onEvent(Event &e);
  void onDestroy();

  // Entity management
  Entity createEntity(const std::string &name = "Entity");
  void destroyEntity(Entity entity);
  void clear();

  // Named entity lookup
  Entity findEntityByName(const std::string &name);

  // Camera management
  void setMainCamera(Entity camera);
  Entity getMainCamera();

  // Getters
  const std::string &getName() const { return name_; }
  entt::registry &getRegistry() { return registry_; }

  // Setters
  template <typename T, typename... Args> T &addSystem(Args &&...args) {
    auto system = std::make_unique<T>(std::forward<Args>(args)...);
    systems_.push_back(std::move(system));
    return static_cast<T &>(*systems_.back());
  }

private:
  friend class PC_API Entity;

  std::string name_;
  entt::registry registry_;
  std::vector<std::unique_ptr<System>> systems_;
};


} // namespace Perceptral
