cmake_minimum_required(VERSION 3.23)
project(CloudAnnotationCore VERSION 1.0.0 LANGUAGES CXX C)

# Options
set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(external)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)

option(BUILD_EXAMPLES ON)

# Core library source files
set(CORE_SOURCES
    src/core/Log.cpp
    # Core
    src/core/Window.cpp
    src/core/EventManager.cpp
    src/core/Application.cpp
    src/core/Input.cpp
    src/core/LayerStack.cpp
    src/core/layers/GameLayer.cpp
    src/core/Time.cpp
    src/core/AssetManager.cpp

    # New renderer abstraction
    src/renderer/RenderAPI.cpp
    src/renderer/Renderer.cpp
    src/renderer/Buffer.cpp
    src/renderer/VertexArray.cpp
    src/renderer/Shader.cpp
    src/renderer/ShaderLibrary.cpp
    src/renderer/features/BackgroundRenderer.cpp
    src/renderer/features/GridAxisLineRenderer.cpp

    # OpenGL Platform
    src/platform/opengl/OpenGLRenderAPI.cpp
    src/platform/opengl/OpenGLBuffer.cpp
    src/platform/opengl/OpenGLVertexArray.cpp
    src/platform/opengl/OpenGLShader.cpp

    # Scene (EnTT ECS)
    src/scene/Scene.cpp
    src/scene/SceneManager.cpp
    src/scene/systems/CameraSystem.cpp
    src/scene/systems/CameraControllerSystem.cpp
    src/scene/systems/RenderSystem.cpp
    src/scene/systems/ScriptSystem.cpp

    src/core/assets/PointCloud.cpp

    # I/O

    # UI
    src/ui/ImGuiLayer.cpp
)

set(CORE_HEADERS
    include/Perceptral/core/Log.h
    # Core
    include/Perceptral/core/Window.h
    include/Perceptral/core/EventManager.h
    include/Perceptral/core/Application.h
    include/Perceptral/core/Input.h
    include/Perceptral/core/KeyCodes.h
    include/Perceptral/core/Event.h
    include/Perceptral/core/Time.h
    include/Perceptral/core/AssetManager.h

    # New renderer abstraction
    include/Perceptral/renderer/RenderAPI.h
    include/Perceptral/renderer/Renderer.h
    include/Perceptral/renderer/Shader.h
    include/Perceptral/renderer/ShaderLibrary.h
    include/Perceptral/renderer/Buffer.h
    include/Perceptral/renderer/VertexArray.h
    include/Perceptral/renderer/features/BackgroundRenderer.h
    include/Perceptral/renderer/features/GridAxisLineRenderer.h

    # OpenGL Platform
    include/Perceptral/platform/opengl/OpenGLRenderAPI.h
    include/Perceptral/platform/opengl/OpenGLBuffer.h
    include/Perceptral/platform/opengl/OpenGLVertexArray.h
    include/Perceptral/platform/opengl/OpenGLShader.h

    # Scene (EnTT ECS)
    include/Perceptral/scene/Scene.h
    include/Perceptral/scene/Entity.h
    include/Perceptral/scene/SceneManager.h
    include/Perceptral/scene/systems/CameraSystem.h
    include/Perceptral/scene/systems/CameraControllerSystem.h
    include/Perceptral/scene/systems/RenderSystem.h
    include/Perceptral/scene/systems/ScriptSystem.h

    # I/O

    # UI
    include/Perceptral/ui/ImGuiLayer.h
)


set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

set(IMGUI_HEADERS
    external/imgui/imconfig.h
    external/imgui/imgui.h
    external/imgui/imgui_internal.h
    external/imgui/backends/imgui_impl_glfw.h
    external/imgui/backends/imgui_impl_opengl3.h
    external/imgui/backends/imgui_impl_opengl3_loader.h
)

# Create core library
add_library(perceptral STATIC ${CORE_SOURCES} ${CORE_HEADERS} ${IMGUI_SOURCES} ${IMGUI_HEADERS} external/tinyply/source/tinyply.cpp)
target_compile_definitions(perceptral PRIVATE PC_ENGINE_INTERNAL)
target_compile_definitions(perceptral PRIVATE PC_ENGINE_ASSETS="${CMAKE_CURRENT_SOURCE_DIR}/assets")

target_link_libraries(perceptral
    PUBLIC
        Eigen3::Eigen
        OpenGL::GL
        EnTT::EnTT
        spdlog::spdlog_header_only
        glfw
        glad
        tomlplusplus::tomlplusplus
)

target_include_directories(perceptral
    PUBLIC
       ${CMAKE_CURRENT_SOURCE_DIR}/include
       ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
       ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
       ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyply/source
)

if(${BUILD_EXAMPLES})
    add_executable(app app/main.cpp)
    target_link_libraries(app PRIVATE perceptral)
endif()