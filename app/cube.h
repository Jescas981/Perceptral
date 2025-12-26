#include <Perceptral/Perceptral.h>

class CubeScriptable : public Perceptral::Scriptable {
public:
  void onCreate() override {
    // Create materials
    Perceptral::Material material{"Unlit"};
    material.setColor(Eigen::Vector3f{0.5, 0.5, 0.5});
    material.setShader(Perceptral::Shader::create("assets/shaders/unlit.glsl"));
    getEntity().addComponent<Perceptral::Component::MeshData>(CreateCube());
    getEntity().addComponent<Perceptral::Component::MeshRenderer>(material);
  }

  void onUpdate(Perceptral::DeltaTime dt) override {
    angle_ += speed_ * dt.seconds();
    if (angle_ > 2 * M_PI) {
      angle_ -= 2 * M_PI;
    }
    float x = radius_ * cos(angle_);
    float y = radius_ * sin(angle_);
    getTransform().translation = Eigen::Vector3f(x, y, 0.0f);
    getTransform().rotation =
        Eigen::AngleAxisf(angle_, Eigen::Vector3f::UnitZ());
  }

  void onDestroy() override {}

private:
  Perceptral::Component::MeshData CreateCube() {
    Perceptral::Component::MeshData meshData;
    meshData.vertices = {{-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f},
                         {0.5f, 0.5f, -0.5f},   {-0.5f, 0.5f, -0.5f},
                         {-0.5f, -0.5f, 0.5f},  {0.5f, -0.5f, 0.5f},
                         {0.5f, 0.5f, 0.5f},    {-0.5f, 0.5f, 0.5f}};
    meshData.indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 0, 3, 7, 7, 4, 0,
                        1, 5, 6, 6, 2, 1, 0, 1, 5, 5, 4, 0, 3, 2, 6, 6, 7, 3};
    meshData.isDirty = true;
    return meshData;
  }

private:
  float angle_{0.0f};
  float radius_{5.0f};
  float speed_{2.0f};
};