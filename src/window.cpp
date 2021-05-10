#include "window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <glm/glm.hpp>

Window::Window() {
    if (!glfwInit())
        throw std::runtime_error{"Failed to initialize GLFW!"};

    mWindow = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!mWindow) {
        glfwTerminate();
        throw std::runtime_error{"Failed to create window!"};
    }

    glfwMakeContextCurrent(mWindow);
}

Window& Window::get() {
    static Window instance{};
    return instance;
}

void Window::loop() {
    while (!glfwWindowShouldClose(mWindow))
        render();
}

void Window::render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::vec3 a{1.f, 2.f, 3.f};

    glfwSwapBuffers(mWindow);
    glfwPollEvents();
}

Window::~Window() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}