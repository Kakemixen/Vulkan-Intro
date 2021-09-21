#include "movement_system.hpp"
#include "game_object.hpp"

#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyMovementSystem::MyMovementSystem(GLFWwindow* window)
    : window(window)
{ }

void MyMovementSystem::updateTick(
        std::vector<MyGameObject>& gameObjects,
        float timeDelta)
{
    glm::vec3 translation = glm::vec3(0.f, 0.f, -0.f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        translation += glm::vec3(1.f, 0.f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        translation += glm::vec3(0.f, 1.f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        translation += glm::vec3(-1.f, 0.f, 0.f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        translation += glm::vec3(0.f, -1.f, 0.f);

    if (!(translation == glm::vec3(0.f)))
        translation = glm::normalize(translation);

    translation *= 10.f;

    for (auto& gameObject : gameObjects) {
        gameObject.transform.matrix = glm::translate(
                gameObject.transform.matrix, 
                timeDelta * translation);
    }
}
