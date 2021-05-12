#pragma once

#include <string>
#include <map>

struct GLFWwindow;
class Shader;

class Window {
    friend int main();
private:
    Window();

public:
    Window(const Window&) = delete;

    void operator=(const Window&) = delete;

    static Window& get();
    void init();
    void render();
    template <typename F>
    void render(F&& func) {
        func();
        renderFuncFinish();
    }
    void loop();

    bool shouldClose() const;

    Shader& shader(std::string&& name);

private:
    GLFWwindow* mWindow;
    std::map<std::string, Shader> mShaders;

    void renderFuncFinish();

    ~Window();
};