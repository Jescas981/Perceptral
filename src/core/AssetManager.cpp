#include <Perceptral/core/AssetManager.h>
#include <stdexcept>

namespace Perceptral {

std::unordered_map<std::string, AssetManager::CachedAsset> AssetManager::cache_;

std::unordered_map<std::string, std::filesystem::path> AssetManager::roots_;

void AssetManager::init() {
#ifdef PC_ENGINE_ASSETS
  addRoot("core", PC_ENGINE_ASSETS);
#endif
}

void AssetManager::addRoot(const std::string &name,
                           const std::filesystem::path &path) {
  roots_[name] = path;
}

std::filesystem::path AssetManager::resolve(const std::string &virtualPath) {
  const auto colon = virtualPath.find(':');
  if (colon == std::string::npos || colon == 0 ||
      colon == virtualPath.size() - 1)
    throw std::runtime_error("Invalid virtual path: " + virtualPath);

  const std::string rootName = virtualPath.substr(0, colon);
  std::string relative = virtualPath.substr(colon + 1);

  auto it = roots_.find(rootName);
  if (it == roots_.end())
    throw std::runtime_error("Unknown asset root: " + rootName);

  // Remove accidental leading slash
  if (!relative.empty() && (relative[0] == '/' || relative[0] == '\\'))
    relative.erase(0, 1);

  return it->second / relative;
}

} // namespace Perceptral