# pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class MyTransformComponent 
{
public:
    MyTransformComponent();
    MyTransformComponent(glm::vec3 location);
    MyTransformComponent(
            glm::vec3 location,
            glm::vec3 scale,
            glm::quat rotation);

    glm::mat4 getMatrix() const;

    void translate(glm::vec3 translation);
    void scale(glm::vec3 scale);
    void rotate(glm::quat rotation);

private:
    glm::vec3 m_location;
    glm::vec3 m_scale;
    glm::quat m_rotation;
};

