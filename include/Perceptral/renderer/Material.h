#pragma once
#include <Eigen/Core>
#include <Perceptral/core/Macros.h>
#include <memory>
#include <string>

namespace Perceptral {

class Shader; // Forward declaration
using ShaderPtr = std::shared_ptr<Shader>;

class PC_API Material {
public:
  Material(const std::string &name = "DefaultMaterial") : m_name(name) {}
  virtual ~Material() = default;

  void setName(const std::string &name) { m_name = name; }
  const std::string &getName() const { return m_name; }

  void setColor(const Eigen::Vector3f &color) { m_color = color; }
  const Eigen::Vector3f &getColor() const { return m_color; }

  void setOpacity(float opacity) {
    m_opacity = std::clamp(opacity, 0.0f, 1.0f);
  }
  float getOpacity() const { return m_opacity; }

  void setShader(ShaderPtr shader) { m_shader = shader; }
  ShaderPtr getShader() const { return m_shader; }

private:
  std::string m_name;
  Eigen::Vector3f m_color{0.8f, 0.8f, 0.8f};
  float m_opacity{1.0f};
  ShaderPtr m_shader;
};

using MaterialPtr = std::shared_ptr<Material>;

} // namespace Perceptral