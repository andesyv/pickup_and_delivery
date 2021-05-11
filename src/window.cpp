#include "window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include <iostream>
#include "shader.h"

static const char COMPUTE_CS[] = {R"MSTR(@compute-cs.glsl@)MSTR"};

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
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    mWindow = glfwCreateWindow(640, 480, "Hello Meta-Heuristics!", nullptr, nullptr);
    if (!mWindow) {
        glfwTerminate();
        throw std::runtime_error{"Failed to create window!"};
    }

    glfwMakeContextCurrent(mWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error{"Failed to initialize GLAD"};

    init();
}

Window& Window::get() {
    static Window window{};
    return window;
}

void Window::init() {
#ifndef NDEBUG
    // OpenGL Debugger callback:
    static auto debugMessageCallback = [](
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
        std::cout << "GL DEBUG: " << message << std::endl;;
    };
    glDebugMessageCallback(debugMessageCallback, nullptr);
#endif

    glClearColor(0.3f, 0.5f, 0.3f, 0.f);

    // Init shaders:
    // mShaders.emplace("compute", std::move(Shader{{
    //     {GL_COMPUTE_SHADER, "./src/compute-cs.glsl"}
    // }}));
    mShaders.emplace("compute", std::move(Shader{{
        {GL_COMPUTE_SHADER, COMPUTE_CS}
    }}));
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

Shader& Window::shader(std::string&& name) {
    assert(mShaders.contains("name"));
    return mShaders.at(name);
}

Window::~Window() {
    mShaders.clear();
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}