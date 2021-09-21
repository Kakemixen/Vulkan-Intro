#pragma once

#include <vector>

class MyGameObject;

class MyMovementSystem
{
public:
    void updateTick(std::vector<MyGameObject>& gameObjects,
            float timeDelta);
private:
};
