struct GLFWwindow;

class Window {
public:
    Window();

    static Window& get();
    void loop();
    void render();

    ~Window();

private:
    GLFWwindow* mWindow;
};