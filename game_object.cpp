#include "game_object.hpp"
#include "model.hpp"
#include "texture.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyGameObject MyGameObject::createGameObject(
        std::shared_ptr<MyModel> model,
        std::shared_ptr<MyTexture> texture)
{
    static uint32_t currentId;
    return MyGameObject{currentId++, model, texture};
}

MyGameObject::MyGameObject(uint32_t id,
        std::shared_ptr<MyModel> model,
        std::shared_ptr<MyTexture> texture)
    : id(id),
      model(model),
      texture(texture)
{
    transform.matrix = glm::translate(transform.matrix, glm::vec3(-2.f, -2.f, -2.f));
}

uint32_t MyGameObject::getId()
{
    return id;
}

void MyGameObject::updateTick(float timeDelta)
{
    transform.matrix = glm::translate(transform.matrix, timeDelta * glm::vec3(0.f, 0.f, -0.2f));
    transform.matrix = glm::rotate(transform.matrix, timeDelta * glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
}
