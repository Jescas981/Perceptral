#pragma once

#include <Perceptral/core/Macros.h>
#include <entt/entt.hpp>

namespace Perceptral {

class PC_API Entity {
public:
  Entity() = default;
  Entity(entt::entity handle, entt::registry *registry)
      : entityHandle_(handle), registry_(registry) {};
  Entity(const Entity &other) = default;
  Entity &operator=(const Entity &other) = default;

  template <typename T, typename... Args> T &addComponent(Args &&...args);

  template <typename T> T &getComponent();

  template <typename T> const T &getComponent() const;

  template <typename T> bool hasComponent() const;

  template <typename T> void removeComponent();

  // Validity check
  operator bool() const {
    return entityHandle_ != entt::null && registry_ != nullptr;
  }
  operator entt::entity() const { return entityHandle_; }
  operator uint32_t() const { return static_cast<uint32_t>(entityHandle_); }

  // Comparison operators
  bool operator==(const Entity &other) const {
    return entityHandle_ == other.entityHandle_ && registry_ == other.registry_;
  }

  bool operator!=(const Entity &other) const { return !(*this == other); }

private:
  entt::entity entityHandle_{entt::null};
  entt::registry *registry_{nullptr};
};

template <typename T, typename... Args>
inline T &Entity::addComponent(Args &&...args) {
  return registry_->template emplace<T>(entityHandle_,
                                        std::forward<Args>(args)...);
}

template <typename T> inline T &Entity::getComponent() {
  return registry_->template get<T>(entityHandle_);
}

template <typename T> inline bool Entity::hasComponent() const {
  return registry_->template all_of<T>(entityHandle_);
}

template <typename T> inline void Entity::removeComponent() {
  registry_->template erase<T>(entityHandle_);
}

} // namespace Perceptral
