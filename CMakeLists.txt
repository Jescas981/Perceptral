cmake_minimum_required(VERSION 3.23)
project(CloudAnnotationCore VERSION 1.0.0 LANGUAGES CXX C)

# Options
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
add_library(perceptral STATIC ${CORE_SOURCES} ${CORE_HEADERS} ${IMGUI_SOURCES} ${IMGUI_HEADERS})

target_include_directories(perceptral
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(perceptral
    PUBLIC
        Eigen3::Eigen
        OpenGL::GL
        EnTT::EnTT
        spdlog::spdlog_header_only
        glfw
        glad
)

target_include_directories(perceptral
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/imgui>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends>
        $<INSTALL_INTERFACE:include/imgui>
        $<INSTALL_INTERFACE:include/imgui/backends>
)

target_compile_definitions(perceptral PRIVATE PC_ENGINE_INTERNAL)

# Compiler warnings
if(MSVC)
    target_compile_options(perceptral PRIVATE /W4)
else()
    target_compile_options(perceptral PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy assets to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

# # Installation
# install(TARGETS perceptral glad glfw EnTT spdlog_header_only happly
#     EXPORT CloudAnnotationCoreTargets
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
#     INCLUDES DESTINATION include
# )

# install(DIRECTORY include/ DESTINATION include)
# install(DIRECTORY shaders/ DESTINATION share/perceptral/shaders)

# # Install external dependencies headers that are needed
# install(DIRECTORY external/imgui/ DESTINATION include/imgui
#     FILES_MATCHING PATTERN "*.h")
# install(DIRECTORY external/imgui/backends/ DESTINATION include/imgui/backends
#     FILES_MATCHING PATTERN "*.h")

# # Export targets for find_package
# install(EXPORT CloudAnnotationCoreTargets
#     FILE CloudAnnotationCoreTargets.cmake
#     NAMESPACE CloudAnnotationCore::
#     DESTINATION lib/cmake/CloudAnnotationCore
# )

# # Create and install package config file
# include(CMakePackageConfigHelpers)

# configure_package_config_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CloudAnnotationCoreConfig.cmake.in
#     ${CMAKE_CURRENT_BINARY_DIR}/CloudAnnotationCoreConfig.cmake
#     INSTALL_DESTINATION lib/cmake/CloudAnnotationCore
# )

# write_basic_package_version_file(
#     ${CMAKE_CURRENT_BINARY_DIR}/CloudAnnotationCoreConfigVersion.cmake
#     VERSION ${PROJECT_VERSION}
#     COMPATIBILITY SameMajorVersion
# )

# install(FILES
#     ${CMAKE_CURRENT_BINARY_DIR}/CloudAnnotationCoreConfig.cmake
#     ${CMAKE_CURRENT_BINARY_DIR}/CloudAnnotationCoreConfigVersion.cmake
#     DESTINATION lib/cmake/CloudAnnotationCore
# )

if(${BUILD_EXAMPLES})
    add_executable(app app/main.cpp)
    target_link_libraries(app PRIVATE perceptral)
endif()