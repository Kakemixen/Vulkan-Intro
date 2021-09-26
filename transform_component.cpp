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
    m_rotation * rotation;
}

glm::mat4 MyTransformComponent::getMatrix() const
{
    const glm::quat& q = m_rotation;
    glm::mat4 mat{
        {
            (1 - 2*pow(q.y,2) - 2*pow(q.z,2)) * m_scale.x, 
            (2*q.x*q.y + 2*q.w*q.z),
            (2*q.x*q.z + 2*q.w*q.y),
            0.f
        },
        {
            (2*q.x*q.y + 2*q.w*q.z),
            (1 - 2*pow(q.x,2) - 2*pow(q.z,2)) * m_scale.y, 
            (2*q.y*q.z + 2*q.w*q.x),
            0.f
        },
        {
            (2*q.x*q.z + 2*q.w*q.y),
            (2*q.y*q.z + 2*q.w*q.x),
            (1 - 2*pow(q.x,2) - 2*pow(q.y,2)) * m_scale.z, 
            0.f
        },
        {m_location.x, m_location.y, m_location.z, 1}
    };
    //std::cout << glm::to_string(mat) << "\n";
    return mat;
}
