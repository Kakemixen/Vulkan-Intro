#include "movement_system.hpp"
#include "game_object.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void MyMovementSystem::updateTick(
        std::vector<MyGameObject>& gameObjects,
        float timeDelta)
{
    for (auto& gameObject : gameObjects) {
        gameObject.transform.matrix = glm::translate(
                gameObject.transform.matrix, 
                timeDelta * glm::vec3(0.f, 0.f, -0.2f));
        gameObject.transform.matrix = glm::rotate(
                gameObject.transform.matrix, 
                timeDelta * glm::radians(90.f), 
                glm::vec3(0.f, 0.f, 1.f));
    }
}
