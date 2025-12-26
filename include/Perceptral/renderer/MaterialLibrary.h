#pragma once

#include <Perceptral/renderer/Shader.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace Perceptral {

class PC_API MaterialLibrary {
public:
    std::shared_ptr<Material> create(const std::string& name, std::shared_ptr<Shader> shader) {
        auto mat = std::make_shared<Material>();
        mat->shader = shader;
        materials[name] = mat;
        return mat;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
};

} // namespace Perceptral
