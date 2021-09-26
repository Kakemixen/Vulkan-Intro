#pragma once

#include "transform_component.hpp"

#include <memory>

class MyModel;
class MyTexture;

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
    MyTransformComponent transform{};

private:
    MyGameObject(uint32_t id,
            std::shared_ptr<MyModel> model,
            std::shared_ptr<MyTexture> texture);
    MyGameObject(uint32_t id);

    uint32_t id;
};
