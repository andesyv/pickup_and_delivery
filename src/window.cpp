#include "window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <iostream>

Window::Window() {
    static auto errCallback = [](int errnum, const char * errmsg){
        std::cout << "ERR: " << errnum << ": " << errmsg;
    };
    glfwSetErrorCallback(errCallback);

    if (!glfwInit())
        throw std::runtime_error{"Failed to initialize GLFW!"};

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	// glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(640, 480, "Hello Meta-Heuristics!", nullptr, nullptr);
    if (!mWindow) {
        glfwTerminate();
        throw std::runtime_error{"Failed to create window!"};
    }

    glfwMakeContextCurrent(mWindow);

    init();
}

Window& Window::get() {
    static Window window{};
    return window;
}

void Window::init() {
    glClearColor(0.3f, 0.5f, 0.3f, 0.f);
}

void Window::render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::vec3 a{1.f, 2.f, 3.f};

    glfwSwapBuffers(mWindow);
    glfwPollEvents();
}

void Window::loop() {
    while (!glfwWindowShouldClose(mWindow))
        render();
}

Window::~Window() {
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}