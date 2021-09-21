#pragma once

#include <vector>

class GLFWwindow;
class MyGameObject;

class MyMovementSystem
{
public:
    MyMovementSystem(GLFWwindow* window);

    void updateTick(std::vector<MyGameObject>& gameObjects,
            float timeDelta);
private:
    GLFWwindow* window;
};
