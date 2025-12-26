#pragma once

#include <Perceptral/core/Macros.h>

namespace Perceptral {

namespace Resource {
class Resource;
class Shader;
class Material;
} // namespace Resource

class PC_API ResourceManager {
public:
  ResourceManager(const ResourceManager &) = delete;
  ResourceManager &operator=(const ResourceManager &) = delete;

//   template <typename T>
//   ResourceHandle<T> load(const std::string &path, bool forceReload = false);


};

} // namespace Perceptral