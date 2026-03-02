#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace Perceptral {
template <typename T> struct AssetTraits {
  static constexpr const char *Folder = "";
};

class AssetManager {
public:
  static void init();

  static void addRoot(const std::string &name,
                      const std::filesystem::path &path);

  template <typename T>
  static std::shared_ptr<T> load(const std::string &virtualPath);

private:
  static std::filesystem::path resolve(const std::string &virtualPath);

  struct CachedAsset {
    std::shared_ptr<void> asset;
    std::type_index type;
  };

  static std::unordered_map<std::string, CachedAsset> cache_;
  static std::unordered_map<std::string, std::filesystem::path> roots_;
};

template <typename T>
std::shared_ptr<T> AssetManager::load(const std::string &virtualPath) {
  auto it = cache_.find(virtualPath);
  if (it != cache_.end()) {
    if (it->second.type != std::type_index(typeid(T)))
      throw std::runtime_error("Asset type mismatch for: " + virtualPath);

    return std::static_pointer_cast<T>(it->second.asset);
  }

  // Resolve root and relative path
  auto colon = virtualPath.find(':');
  if (colon == std::string::npos)
    throw std::runtime_error("Invalid virtual path: " + virtualPath);

  std::string rootName = virtualPath.substr(0, colon);
  std::string relative = virtualPath.substr(colon + 1);

  auto rootIt = roots_.find(rootName);
  if (rootIt == roots_.end())
    throw std::runtime_error("Unknown asset root: " + rootName);

  // Apply type-based folder
  std::filesystem::path fullPath =
      rootIt->second / AssetTraits<T>::Folder / relative;

  auto asset = T::createFromFile(fullPath.string());

  cache_.emplace(virtualPath, CachedAsset{asset, typeid(T)});

  return asset;
}

} // namespace Perceptral
