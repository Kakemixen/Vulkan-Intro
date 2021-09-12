#pragma once

typedef struct GLFWwindow GLFWwindow;

// forward from vulkan
typedef struct VkExtent2D VkExtent2D;

class MyWindow
{
public:
    MyWindow();
    ~MyWindow();
    MyWindow(const MyWindow&) = delete;
    MyWindow operator=(const MyWindow&) = delete;

    bool shouldClose();
    VkExtent2D getExtent();
    bool wasResized();
    void resetResizedFlag();


    GLFWwindow* window;
private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    int width, height;
    bool framebufferResized = false;
};
