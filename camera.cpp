#include "camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyCamera::MyCamera() {}

MyCamera::MyCamera(glm::vec3 position, float ar)
    : position(position)
{
    view = glm::translate(glm::mat4(1.f), position);
    projection = glm::perspective(glm::radians(45.f), 
            ar, 0.1f, 10.f);
    projection[1][1] *= -1;
}

MyCamera::MyCamera(glm::vec3 position, 
        glm::vec3 direction,
        glm::vec3 up,
        float ar)
    : position(position),
      direction(direction),
      up(up)
{
    view = glm::lookAt(position, direction + position, up);
    projection = glm::perspective(glm::radians(45.f), 
            ar, 0.1f, 10.f);
    projection[1][1] *= -1;
}

void MyCamera::lookAt(glm::vec3 position, glm::vec3 up)
{
    view = glm::lookAt(this->position, position, up);
}

void MyCamera::lookIn(glm::vec3 direction, glm::vec3 up)
{
    view = glm::lookAt(this->position, direction + this->position, up);
}

void MyCamera::move(glm::vec3 translation)
{
    position += translation;
    view = glm::lookAt(position, direction + position, up);
}

void MyCamera::updateAr(float newAr)
{
    projection = glm::perspective(glm::radians(45.f), 
            newAr, 0.1f, 10.f);
    projection[1][1] *= -1;
}


glm::mat4 MyCamera::getView() const
{
    return view;
}

glm::mat4 MyCamera::getProjection() const
{
    return projection;
}
