#include "transform_component.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <glm/gtx/string_cast.hpp>

MyTransformComponent::MyTransformComponent()
    : m_location(0.f),
      m_scale(1.f),
      m_rotation(1.f, 0.f, 0.f, 0.f)
{ }

MyTransformComponent::MyTransformComponent(glm::vec3 location)
    : m_location(location),
      m_scale(1.f),
      m_rotation(1.f, 0.f, 0.f, 0.f)
{ }

MyTransformComponent::MyTransformComponent(
        glm::vec3 location,
        glm::vec3 scale,
        glm::quat rotation)
    : m_location(location),
      m_scale(scale),
      m_rotation(rotation)
{ }

void MyTransformComponent::translate(glm::vec3 translation)
{
    m_location += translation;
}

void MyTransformComponent::scale(glm::vec3 scale)
{
    m_scale *= scale;
}

void MyTransformComponent::rotate(glm::quat rotation)
{
    if (glm::length(rotation) != 1) { // TODO don't need the square root for checking !=1
        rotation = glm::normalize(rotation);
        //std::cout << "non unit quat!" << glm::to_string(rotation) << "\n";
    }
    m_rotation = rotation * m_rotation;
}

glm::mat4 MyTransformComponent::getMatrix() const
{
    const glm::quat& q = m_rotation;
    glm::mat4 mat{
        {
            (1 - 2*pow(q.y,2) - 2*pow(q.z,2)) * m_scale.x, 
            (2*q.x*q.y + 2*q.w*q.z) * m_scale.x,
            (2*q.x*q.z - 2*q.w*q.y) * m_scale.x,
            0.f
        },
        {
            (2*q.x*q.y - 2*q.w*q.z) * m_scale.y,
            (1 - 2*pow(q.x,2) - 2*pow(q.z,2)) * m_scale.y, 
            (2*q.y*q.z + 2*q.w*q.x) * m_scale.y,
            0.f
        },
        {
            (2*q.x*q.z + 2*q.w*q.y) * m_scale.z,
            (2*q.y*q.z - 2*q.w*q.x) * m_scale.z,
            (1 - 2*pow(q.x,2) - 2*pow(q.y,2)) * m_scale.z, 
            0.f
        },
        {m_location.x, m_location.y, m_location.z, 1}
    };
    return mat;
}
