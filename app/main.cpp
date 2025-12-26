#include "cube.h"
#include <Perceptral/EntryPoint.h>
#include <Perceptral/Perceptral.h>
#include <Perceptral/core/layers/GameLayer.h>

class MyExampleApp : public Perceptral::Application {
public:
  MyExampleApp() {}

  void onCreate() override {
    // Create scene
    auto scene = std::make_shared<Perceptral::Scene>("MainScene");
    getSceneManager().pushScene(scene);

    // Create camera controller
    auto camEnt = scene->getMainCamera();
    camEnt.addComponent<Perceptral::Component::OrbitCameraController>();

    auto cubeEnt = scene->createEntity("CubeScript");


    auto &nsc = cubeEnt.addComponent<Perceptral::Component::NativeScript>();
    Perceptral::BindNativeScript<CubeScriptable>(nsc);
    // Add layer
    getLayerStack().pushLayer(std::make_unique<Perceptral::GameLayer>(*scene));
  }
};

Perceptral::Application *Perceptral::createApplication() {
  return new MyExampleApp();
}