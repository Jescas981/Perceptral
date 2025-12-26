#pragma once

#include <Perceptral/core/Application.h>
#include <Perceptral/core/Log.h>
#include <exception>

extern Perceptral::Application* Perceptral::createApplication();

int main(int argc, char** argv) {
    try {
        auto* app = Perceptral::createApplication();
        if (!app) {
            PC_ERROR("Failed to create application");
            return -1;
        }
        
        if (!app->initialize()) {
            PC_ERROR("Failed to initialize");
            delete app;
            return -1;
        }
        
        app->run();
        delete app;
        
        return 0;
        
    } catch (const std::exception& e) {
        PC_ERROR("Fatal error: {}", e.what());
        return -1;
    }
}