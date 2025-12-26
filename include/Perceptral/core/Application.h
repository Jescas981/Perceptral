#pragma once

#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Event.h>
#include <Perceptral/core/EventManager.h>
#include <Perceptral/core/LayerStack.h>
#include <Perceptral/core/Macros.h>
#include <Perceptral/core/Window.h>
#include <Perceptral/scene/Entity.h>
#include <Perceptral/scene/SceneManager.h>
#include <Perceptral/scene/Components.h>
#include <Perceptral/ui/ImGuiLayer.h>
#include <memory>
#include <string>

namespace Perceptral {

class PC_API Renderer;

struct ApplicationConfig {
  int windowWidth = 1280;
  int windowHeight = 720;
  std::string windowTitle = "Perceptral Engine";
  bool fullscreen = false;
  bool vsync = true;
  int maxFPS = 0; // 0 = unlimited
};

class PC_API Application {
public:
  Application();
  virtual ~Application();

  // Lifecycle
  bool initialize();
  void run();
  void shutdown();

  virtual void onEvent(Event &e);

  SceneManager &getSceneManager() { return sceneManager_; }

  // Getters
  Window *getWindow() const { return window_.get(); }
  EventManager *getEventManager() const { return eventManager_.get(); }

  ImGuiLayer *getImGuiLayer() const { return imguiLayer_.get(); }
  LayerStack &getLayerStack() { return layerStack_; }

  // Control
  void stop() { running_ = false; }
  bool isRunning() const { return running_; }

  virtual ApplicationConfig getDefaultConfig() { return ApplicationConfig{}; };

  // Singleton access
  static Application &get() { return *s_Instance; }

protected:
  virtual void onCreate() {}

  virtual void onUpdate(DeltaTime deltaTime) { UNUSED(deltaTime); }
  virtual void onDestroy() {}
  virtual void onRender() {}
  virtual void onImGuiRender() {}
  virtual void onShutdown() {}

  // Default event handlers
  bool onWindowClose(WindowCloseEvent &e);
  bool onWindowResize(WindowResizeEvent &e);

private:
  static Application *s_Instance;

  LayerStack layerStack_;
  ApplicationConfig config_;
  std::unique_ptr<Window> window_;
  std::unique_ptr<EventManager> eventManager_;
  std::unique_ptr<ImGuiLayer> imguiLayer_;
  SceneManager sceneManager_;

  bool running_;
  float lastFrameTime_;
};

extern Application *createApplication();

} // namespace Perceptral
