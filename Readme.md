# A basic engine with agnostic-platform for rendering

# 🌀 Perceptral Engine

Perceptral is a high-performance, cross-platform 3D game engine built in C++ focusing on modularity, modern OpenGL rendering, and an intuitive Entity Component System (ECS).

Imagine your engine video playing here: high-speed rendering, smooth orbit controls, and dynamic scripting.

## ✨ Core Features

### 🛠️ Architecture & ECS

Native Scripting: Hook into the engine lifecycle with onCreate, onUpdate, and onDestroy using the Scriptable interface.

Entity Component System: Clean separation of data and logic via Perceptral::Component and specialized Systems.

Layer Stack Architecture: Flexible application flow management (GameLayer, ImGuiLayer, etc.).

###  🎨 Graphics & Rendering

Modern OpenGL Backend: Fully abstracted Render API including Vertex Arrays, Buffers, and Shaders.

Material System: Sophisticated Material and MaterialLibrary for managing unlit and complex shaders.

Built-in Render Features: Integrated Grid/Axis visualizers and Background rendering.

### 🕹️ Tools & Input

Orbit Camera Controller: Immediate-mode camera movement for debugging and scene inspection.

Event System: Robust EventManager for handling window, keyboard, and mouse inputs.

ImGui Integration: Ready-to-use UI layer for engine tooling.

## 🚀 Quick Start: Spinning a Cube

Creating interactive objects in Perceptral is straightforward. Here is the implementation for a scriptable spinning cube:

```
#include <Perceptral/Perceptral.h>

class CubeScriptable : public Perceptral::Scriptable {
public:
    void onCreate() override {
        // Initialize Material
        Perceptral::Material material{"Unlit"};
        material.setColor(Eigen::Vector3f{0.5f, 0.5f, 0.5f});
        material.setShader(Perceptral::Shader::create("assets/shaders/unlit.glsl"));

        // Add Components
        getEntity().addComponent<Perceptral::Component::MeshData>(CreateCube());
        getEntity().addComponent<Perceptral::Component::MeshRenderer>(material);
    }

    void onUpdate(Perceptral::DeltaTime dt) override {
        angle_ += speed_ * dt.seconds();
        
        // Transform Logic
        float x = radius_ * cos(angle_);
        float y = radius_ * sin(angle_);
        
        getTransform().translation = Eigen::Vector3f(x, y, 0.0f);
        getTransform().rotation = Eigen::AngleAxisf(angle_, Eigen::Vector3f::UnitZ());
    }

private:
    Perceptral::Component::MeshData CreateCube() {
        Perceptral::Component::MeshData meshData;
        // ... Define vertices and indices ...
        meshData.isDirty = true;
        return meshData;
    }

    float angle_{0.0f}, radius_{5.0f}, speed_{2.0f};
};
```

## Demo Video

[![Watch the video](https://img.youtube.com/vi/oE6-vHxPZ0I/0.jpg)](https://youtu.be/oE6-vHxPZ0I)


📂 Project Structure

```
include/Perceptral
├── core            # Application logic, Events, Windowing, Time
├── renderer        # Materials, Shaders, RenderAPI (OpenGL)
├── scene           # ECS, Entity, SceneManager, Scripting
├── io              # PLY Loaders, Image Exporters
├── platform        # OpenGL specific implementations
├── ui              # ImGui Integration
└── math            # Eigen-based transform utilities
```

🛠️ Requirements

* C++17 or higher

* OpenGL 4.5+

* Eigen 3 (for Math/Linear Algebra)

* CMake (Build System)

📜 License

Perceptral is licensed under the MIT License. See LICENSE for more information.