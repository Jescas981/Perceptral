#include "Perceptral/scene/systems/RenderSystem.h"
#include <Perceptral/core/Log.h>
#include <Perceptral/core/layers/GameLayer.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/scene/systems/CameraControllerSystem.h>
#include <Perceptral/scene/systems/CameraSystem.h>
#include <Perceptral/scene/systems/ScriptSystem.h>

namespace Perceptral {

GameLayer::GameLayer(Scene &scene) : Layer("GameLayer"), scene_(scene) {}

void GameLayer::onAttach() {
  PC_CORE_INFO("GameLayer attached");
  bgRenderer_.initialize(backgroundSettings_);
  axlinesRenderer_.initialize();
  // Add systems
  scene_.addSystem<CameraSystem>();
  scene_.addSystem<CameraControllerSystem>();
  scene_.addSystem<RenderSystem>();
  scene_.addSystem<ScriptSystem>();
}

void GameLayer::onDetach() { PC_CORE_INFO("GameLayer detached"); }

void GameLayer::onRender() {
  auto &camera = scene_.getMainCamera().getComponent<Component::Camera>();
  bgRenderer_.render();
  axlinesRenderer_.render(camera);
}

// void GameLayer::onImGuiRender() {}

} // namespace Perceptral