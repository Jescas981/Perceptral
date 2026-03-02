#include <Eigen/Eigen>
#include <Perceptral/core/assets/PointCloud.h>
#include <fstream>
#include <tinyply.h>

namespace Perceptral {

std::shared_ptr<Asset::PointCloud>
Asset::PointCloud::createFromFile(const std::string &filepath,
                                  std::vector<std::string> fields) {
  auto cloud = std::make_shared<PointCloud>();

  try {
    std::ifstream ss(filepath, std::ios::binary);
    if (!ss.is_open()) {
      PC_ERROR("Could not open file '{}'", filepath);
      return nullptr;
    }

    tinyply::PlyFile file;
    file.parse_header(ss);

    // XYZ is always requested
    std::shared_ptr<tinyply::PlyData> xyzData;
    xyzData = file.request_properties_from_element("vertex", {"x", "y", "z"});

    // Request each extra field individually
    std::unordered_map<std::string, std::shared_ptr<tinyply::PlyData>>
        fieldData;
    for (const auto &field : fields) {
      try {
        fieldData[field] =
            file.request_properties_from_element("vertex", {field});
      } catch (...) {
        PC_WARN("Field '{}' not found in '{}', skipping", field, filepath);
      }
    }

    file.read(ss);

    // Parse XYZ
    const std::size_t pointCount = xyzData->count;
    cloud->points.resize(pointCount);

    // tinyply packs xyz as floats interleaved: x0,y0,z0,x1,y1,z1,...
    const float *raw = reinterpret_cast<const float *>(xyzData->buffer.get());

    if (xyzData->t == tinyply::Type::FLOAT64) {
      const double *raw =
          reinterpret_cast<const double *>(xyzData->buffer.get());
      for (std::size_t i = 0; i < pointCount; ++i)
        cloud->points[i] = Eigen::Vector3f(static_cast<float>(raw[i * 3 + 0]),
                                           static_cast<float>(raw[i * 3 + 1]),
                                           static_cast<float>(raw[i * 3 + 2]));
    } else {
      // Assume FLOAT32
      const float *raw = reinterpret_cast<const float *>(xyzData->buffer.get());
      for (std::size_t i = 0; i < pointCount; ++i)
        cloud->points[i] =
            Eigen::Vector3f(raw[i * 3 + 0], raw[i * 3 + 1], raw[i * 3 + 2]);
    }

    // Parse extra fields
    for (const auto &[name, data] : fieldData) {
      if (!data || data->count == 0)
        continue;

      auto &vec = cloud->fields[name];
      vec.resize(data->count);

      const float *fraw = reinterpret_cast<const float *>(data->buffer.get());
      std::copy(fraw, fraw + data->count, vec.begin());
    }

  } catch (const std::exception &e) {
    PC_ERROR("Failed to load point cloud '{}': {}", filepath, e.what());
    return nullptr;
  }

  return cloud;
}

std::vector<std::string>
Asset::PointCloud::getFieldsFromFile(const std::string &filepath) {
  std::vector<std::string> fields;
  try {
    std::ifstream ss(filepath, std::ios::binary);
    if (!ss.is_open()) {
      PC_ERROR("Could not open file '{}'", filepath);
      return fields;
    }

    tinyply::PlyFile file;
    file.parse_header(ss);

    for (const auto &elem : file.get_elements()) {
      if (elem.name != "vertex")
        continue;

      for (const auto &prop : elem.properties) {
        // Skip xyz, they are always loaded
        if (prop.name == "x" || prop.name == "y" || prop.name == "z")
          continue;
        fields.push_back(prop.name);
      }
    }
  } catch (const std::exception &e) {
    PC_ERROR("Failed to parse PLY header '{}': {}", filepath, e.what());
  }

  return fields;
}
} // namespace Perceptral