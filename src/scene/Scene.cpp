#include "Perceptral/scene/Entity.h"
#include "entt/entity/entity.hpp"
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Macros.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/Scene.h>

namespace Perceptral {

Scene::Scene(const std::string &name) : name_(name) {}

Scene::~Scene() { clear(); }

void Scene::onCreate() {
  for (auto &system : systems_) {
    system->onCreate(*this);
  }
}

void Scene::onUpdate(DeltaTime deltaTime) {
  for (auto &system : systems_) {
    system->onUpdate(*this, deltaTime);
  }
}

void Scene::onImGuiRender() {
  for (auto &system : systems_) {
    system->onImGuiRender(*this);
  }
}

void Scene::onRender() {
  for (auto &system : systems_) {
    system->onRender(*this);
  }
}

void Scene::onEvent(Event &e) {
  for (auto &system : systems_) {
    system->onEvent(*this, e);
  }
}

void Scene::onDestroy() {
  for (auto &system : systems_) {
    system->onDestroy(*this);
  }
}

void Scene::setMainCamera(Entity cameraEntity) {
  // Remove MainCamera tag from any previous camera
  auto previousView = registry_.view<Component::MainCamera>();
  for (auto entity : previousView) {
    registry_.remove<Component::MainCamera>(entity);
  }

  // Add MainCamera component to the new camera entity
  if (!registry_.any_of<Component::MainCamera>(cameraEntity)) {
    registry_.emplace<Component::MainCamera>(cameraEntity);
  }
}

Entity Scene::getMainCamera() {
  auto view = registry_.view<Component::Camera, Component::MainCamera>();
  if (view.begin() != view.end()) {
    return Entity{*view.begin(), &registry_};
  }
  Entity cameraEntity = createEntity("MainCamera");
  cameraEntity.addComponent<Component::Camera>();
  registry_.emplace<Component::MainCamera>(cameraEntity);
  return cameraEntity;
}

Entity Scene::createEntity(const std::string &name) {
  Entity entity = {registry_.create(), &registry_};
  entity.addComponent<Component::Tag>(name);
  entity.addComponent<Component::Transform>();
  entity.addComponent<Component::Relationship>();
  return entity;
}

Entity Scene::createEntityChild(const std::string &name, Entity parent) {
  Entity entity = {registry_.create(), &registry_};
  entity.addComponent<Component::Tag>(name);
  entity.addComponent<Component::Transform>();
  entity.addComponent<Component::Relationship>(parent);
  parent.getComponent<Component::Relationship>().children.emplace_back(entity);
  return entity;
}

void Scene::destroyEntity(Entity entity) { registry_.destroy(entity); }

void Scene::clear() { registry_.clear(); }

Entity Scene::findEntityByName(const std::string &name) {
  auto view = registry_.view<Component::Tag>();
  for (auto entity : view) {
    const Component::Tag &tag = view.get<Component::Tag>(entity);
    if (tag.tag == name) {
      return {entity, &registry_};
    }
  }
  return {};
}

} // namespace Perceptral
