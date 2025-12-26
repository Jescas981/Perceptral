#pragma once

#include <Perceptral/core/Macros.h>
#include <Perceptral/renderer/Shader.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Perceptral {

namespace Resource {

// Resource types
enum class ResourceType { Shader, Mesh, Material, Unknown };

class PC_API Resource {
public:
  virtual ~Resource() = default;

  ResourceType getType() const { return type; }
  const std::string &getPath() const { return path; }
  const std::string &getName() const { return name; }

  bool isLoaded() const { return loaded; }
  virtual bool reload() = 0;

protected:
  ResourceType type = ResourceType::Unknown;
  std::string path;
  std::string name;
  bool loaded = false;
};

} // namespace Resource
} // namespace Perceptral