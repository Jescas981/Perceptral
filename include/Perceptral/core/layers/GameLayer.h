// ============================================
// layers/GameLayer.h
// ============================================
#pragma once
#include <Perceptral/renderer/features/GridAxisLineRenderer.h>
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/layers/Layer.h>
#include <Perceptral/renderer/features/BackgroundRenderer.h>
#include <Perceptral/scene/Scene.h>

namespace Perceptral {

class PC_API GameLayer : public Layer {
public:
  GameLayer(Scene &scene);
  virtual ~GameLayer() = default;

  // Layer lifecycle
  void onAttach() override;
  void onDetach() override;

  // void onUpdate(DeltaTime deltaTime) override;
  void onRender() override;
  // void onEvent(Event &event) override;
  // void onImGuiRender() override;

private:
  BackgroundSettings backgroundSettings_;
  BackgroundRenderer bgRenderer_;
  GridAxisLineRenderer axlinesRenderer_;
  Scene &scene_;
};

} // namespace Perceptral