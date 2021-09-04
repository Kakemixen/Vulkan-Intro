#include "window.hpp"

//libs
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//std
#include <iostream>

//initial values
static const std::uint32_t WIDTH = 800;
static const std::uint32_t HEIGHT = 600;

MyWindow::MyWindow() {}
MyWindow::~MyWindow()
{
    glfwDestroyWindow(window);
}

void MyWindow::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, MyWindow::framebufferResizeCallback);
}

bool MyWindow::shouldClose()
{
    return glfwWindowShouldClose(window);
}

VkExtent2D MyWindow::getExtent()
{
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

void MyWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto myWindow = reinterpret_cast<MyWindow*>(
            glfwGetWindowUserPointer(window));
    std::cout << "resized to: [" << width << ", " << height << "]\n";
    myWindow->framebufferResized = true;
    myWindow->width = width;
    myWindow->height = height;
}

bool MyWindow::wasResized()
{
    return framebufferResized;
}

void MyWindow::resetResizedFlag()
{
    framebufferResized = false;
}
