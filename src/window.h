#pragma once

struct GLFWwindow;

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
    void loop();

private:
    GLFWwindow* mWindow;

    ~Window();
};