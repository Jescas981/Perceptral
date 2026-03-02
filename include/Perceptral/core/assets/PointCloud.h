#pragma once
#include <Perceptral/core/AssetManager.h>
#include <Perceptral/core/Log.h>
#include <Eigen/Eigen>

namespace Perceptral {
namespace Asset {

class PointCloud {
public:
  using Ptr = std::shared_ptr<PointCloud>;

  std::vector<Eigen::Vector3f> points;
  std::vector<float> rgb;  // Optional labels field (interleaved RGB)
  std::vector<int> labels; // Optional labels field

  std::unordered_map<std::string, std::vector<float>> fields;

  inline std::size_t size() const { return points.size(); }

  bool hasField(const std::string &name) const {
    return fields.count(name) > 0;
  }

  bool hasRGB() const { return !rgb.empty(); }

  bool hasLabels() const { return !labels.empty(); }

  bool labelCount() const {
    if (labels.empty())
      return 0;
    return *std::max_element(labels.begin(), labels.end()) + 1;
  }

  static std::shared_ptr<PointCloud>
  createFromFile(const std::string &filepath,
                 std::vector<std::string> fields = {});

  static std::vector<std::string>
  getFieldsFromFile(const std::string &filepath);
};

} // namespace Asset
} // namespace Perceptral