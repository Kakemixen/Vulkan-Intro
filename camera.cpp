#include "camera.hpp"

#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

MyCamera::MyCamera() {}

MyCamera::MyCamera(glm::vec3 position, float ar)
    : position(position)
{
    setView(position, {-1.f, 0.f, 0.f}, {0.f, 0.f, 1.f});
    setPerspectiveProjection(45.f, ar, 0.1f, 10.f);
}

MyCamera::MyCamera(glm::vec3 position, 
        glm::vec3 direction,
        glm::vec3 up,
        float ar)
    : position(position),
      direction(direction),
      up(up)
{
    setView(position, direction, up);
    setPerspectiveProjection(glm::radians(45.f), ar, 0.1f, 10.f);
}

void MyCamera::setView(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};
    this->position = position;
    this->direction = direction;

    view = glm::mat4{1.f};
    view[0][0] = u.x;
    view[1][0] = u.y;
    view[2][0] = u.z;
    view[0][1] = v.x;
    view[1][1] = v.y;
    view[2][1] = v.z;
    view[0][2] = w.x;
    view[1][2] = w.y;
    view[2][2] = w.z;
    view[3][0] = -glm::dot(u, position);
    view[3][1] = -glm::dot(v, position);
    view[3][2] = -glm::dot(w, position);
}

void MyCamera::setPerspectiveProjection(float fovY, 
        float ar, float near, float far)
{
    assert(glm::abs(ar - std::numeric_limits<float>::epsilon()) > 0.0f);
    const float tanHalfFovy = tan(fovY / 2.f);
    this->fovY = fovY;

    projection = glm::mat4{0.f};
    projection[0][0] = 1.f / (ar * tanHalfFovy);
    projection[1][1] = 1.f / (tanHalfFovy);
    projection[2][2] = far / (far - near);
    projection[2][3] = 1.f;
    projection[3][2] = -(far * near) / (far - near);
}

void MyCamera::lookIn(glm::vec3 direction, glm::vec3 up)
{
    setView(this->position, direction, up);
}

void MyCamera::lookAt(glm::vec3 position, glm::vec3 up)
{
    setView(this->position, this->direction + position, up);
}

void MyCamera::move(glm::vec3 translation)
{
    setView(position + translation, direction, up);
}

void MyCamera::setTransform(glm::mat4 transform)
{
    view = transform;
}

void MyCamera::updateAr(float newAr)
{
    const float tanHalfFovy = tan(fovY / 2.f);
    projection[0][0] = 1.f / (newAr * tanHalfFovy);
}

float MyCamera::calculateAspectRatio(uint32_t width, uint32_t height)
{
    return width / (float) height;
}


glm::mat4 MyCamera::getView() const
{
    return view;
}

glm::mat4 MyCamera::getProjection() const
{
    return projection;
}
