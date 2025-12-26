#pragma once
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/System.h>
#include <entt/entt.hpp>

namespace Perceptral {

class RenderSystem : public System {
public:
  void onRender(entt::registry &registry) override;

private:
  void renderMeshes(entt::registry &registry, const Component::Camera &camera);
};

} // namespace Perceptral
