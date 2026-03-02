#pragma once

#include <Perceptral/scene/systems/System.h>

namespace Perceptral {
class Scene;

class ScriptSystem : public System {
public:
  void onCreate(Scene &scene) override;
  void onUpdate(Scene &scene, DeltaTime deltaTime) override;
  void onEvent(Scene &scene, Event &event) override;
  void onDestroy(Scene &scene) override;
  void onImGuiRender(Scene &scene) override;
  void onRender(Scene &scene) override;
};

} // namespace Perceptral