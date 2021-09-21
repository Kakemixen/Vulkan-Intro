#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>

class MyModel;
class MyTexture;

struct TransformComponent 
{
    glm::mat4 matrix{1.f};
    //TODO add convenience functions
};


class MyGameObject 
{
public:
    static MyGameObject createGameObject(
            std::shared_ptr<MyModel> model,
            std::shared_ptr<MyTexture> texture);
    static MyGameObject createGameObject();

    uint32_t getId() const;

    std::shared_ptr<MyModel> model{};
    std::shared_ptr<MyTexture> texture{};
    TransformComponent transform{};

private:
    MyGameObject(uint32_t id,
            std::shared_ptr<MyModel> model,
            std::shared_ptr<MyTexture> texture);
    MyGameObject(uint32_t id);

    uint32_t id;
};
