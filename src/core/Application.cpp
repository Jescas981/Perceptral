#include <Perceptral/core/Application.h>
#include <Perceptral/core/DeltaTime.h>
#include <Perceptral/core/Input.h>
#include <Perceptral/core/Log.h>
#include <Perceptral/core/Macros.h>
#include <Perceptral/core/Time.h>
#include <Perceptral/core/Window.h>
#include <Perceptral/renderer/RenderAPI.h>
#include <Perceptral/renderer/Renderer.h>

namespace Perceptral {

Application *Application::s_Instance = nullptr;

Application::Application() : running_(false), lastFrameTime_(0.0) {
  s_Instance = this;
}

Application::~Application() { shutdown(); }

bool Application::initialize() {
  // Start with default config
  config_ = getDefaultConfig();

  Log::init();
  PC_CORE_INFO("Initializing Application: {}", config_.windowTitle);

  Time::initialize();
  PC_CORE_INFO("Time system initialized");

  window_ = std::make_unique<Window>();

  if (!window_->create(config_.windowWidth, config_.windowHeight,
                       config_.windowTitle)) {
    PC_CORE_ERROR("Failed to create window");
    return false;
  }

  PC_CORE_INFO("Window created: {}x{}", config_.windowWidth,
               config_.windowHeight);

  // Initialize RenderAPI
  Renderer::setRenderAPI(RenderAPI::create());
  Renderer::init();
  Renderer::setClearColor(Eigen::Vector4f(0.1f, 0.1f, 0.1f, 1.0f));
  Renderer::setViewport(0, 0, config_.windowWidth, config_.windowHeight);
  PC_CORE_INFO("Render API initialized");

  // Initialize Input system (platform-independent)
  Input::init(window_->getHandle());
  PC_CORE_DEBUG("Input system initialized");

  // Initialize ImGui layer
  imguiLayer_ = std::make_unique<ImGuiLayer>();
  imguiLayer_->init(window_->getHandle());
  PC_CORE_DEBUG("ImGui layer initialized");

  // Create event manager and set callback
  eventManager_ = std::make_unique<EventManager>();
  eventManager_->initialize(window_->getHandle());
  eventManager_->setEventCallback([this](Event &e) { onEvent(e); });
  PC_CORE_DEBUG("Event manager initialized");

  // User initialization
  onCreate();

  if (auto *scene = sceneManager_.getCurrentScene()) {
    scene->onCreate();
  }

  running_ = true;
  lastFrameTime_ = Time::getTime();

  PC_CORE_INFO("{} initialized successfully", config_.windowTitle);
  return true;
}

void Application::run() {
  while (running_ && !window_->shouldClose()) {
    // Poll events
    window_->pollEvents();
    eventManager_->processEvents();

    // Update
    float currentTime = Time::getTime();
    DeltaTime deltaTime = currentTime - lastFrameTime_;
    lastFrameTime_ = currentTime;

    if (!sceneManager_.getCurrentScene()) {
      PC_CORE_WARN("No scene loaded! Application will run but render nothing.");
    }

    // Update all layers (bottom to top)
    for (Layer *layer : layerStack_) {
      layer->onUpdate(deltaTime);
    }

    if (auto *scene = sceneManager_.getCurrentScene()) {
      scene->onUpdate(deltaTime);
    }

    onUpdate(deltaTime);

    // Render all layers
    Renderer::clear();
    for (Layer *layer : layerStack_) {
      layer->onRender();
    }

    if (auto *scene = sceneManager_.getCurrentScene()) {
      scene->onRender();
    }

    onRender();

    // ImGui rendering
    imguiLayer_->begin();

    for (Layer *layer : layerStack_) {
      layer->onImGuiRender();
    }

    if (auto *scene = sceneManager_.getCurrentScene()) {
      scene->onImGuiRender();
    }

    onImGuiRender();

    imguiLayer_->end();
    window_->swapBuffers();
  }
}

void Application::shutdown() {
  PC_CORE_INFO("Shutting down {}...", config_.windowTitle);

  if (auto *scene = sceneManager_.getCurrentScene()) {
    scene->onDestroy();
  }

  onDestroy();

  // Destroy all scenes
  while (sceneManager_.hasScene()) {
    sceneManager_.popScene();
  }

  for (Layer *layer : layerStack_) {
    layer->onDetach();
  }

  // Clear all layers
  layerStack_.clear();

  if (imguiLayer_) {
    PC_CORE_INFO("Shutting down ImGui layer...");
    imguiLayer_->shutdown();
  }

  PC_CORE_DEBUG("Layers should go down");

  eventManager_.reset();
  Input::shutdown();
  Renderer::shutdown();
  window_.reset();

  onShutdown();

  PC_CORE_INFO("Application shutdown complete");
  Log::shutdown();
}

void Application::onEvent(Event &e) {
  // Let ImGui handle events first
  imguiLayer_->onEvent(e);

  // If ImGui handled it, don't process further
  if (e.isHandled()) {
    return;
  }

  // Forward to layers (top to bottom - reverse order)
  for (auto it = layerStack_.rbegin(); it != layerStack_.rend(); ++it) {
    (*it)->onEvent(e);
    if (e.isHandled()) {
      return;
    }
  }

  // Forward to scene
  if (auto *scene = sceneManager_.getCurrentScene()) {
    scene->onEvent(e);
    if (e.isHandled()) {
      return;
    }
  }

  EventDispatcher dispatcher(e);

  // Dispatch to default handlers
  dispatcher.dispatch<WindowCloseEvent>(
      [this](WindowCloseEvent &event) { return onWindowClose(event); });

  dispatcher.dispatch<WindowResizeEvent>(
      [this](WindowResizeEvent &event) { return onWindowResize(event); });
}

bool Application::onWindowClose(WindowCloseEvent &e) {
  UNUSED(e);
  stop();
  return true;
}

bool Application::onWindowResize(WindowResizeEvent &e) {
  PC_CORE_DEBUG("Window resized: {}x{}", e.getWidth(), e.getHeight());
  Renderer::setViewport(0, 0, e.getWidth(), e.getHeight());
  window_->setHeight(e.getHeight());
  window_->setWidth(e.getWidth());
  return false;
}

} // namespace Perceptral
