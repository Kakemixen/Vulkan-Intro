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

MyGameObject MyGameObject::createGameObject()
{
    static uint32_t currentId;
    return MyGameObject{currentId++};
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

MyGameObject::MyGameObject(uint32_t id)
    : id(id)
{
    transform.matrix = glm::translate(transform.matrix, glm::vec3(-2.f, -2.f, -2.f));
}

uint32_t MyGameObject::getId() const
{
    return id;
}
