#pragma once

#include <glm/fwd.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class MyCamera
{
public:
    MyCamera();
    MyCamera(glm::vec3 position, float ar);
    MyCamera(glm::vec3 position, 
            glm::vec3 direction,
            glm::vec3 up,
            float ar);

    glm::mat4 getView() const;
    glm::mat4 getProjection() const;
    glm::vec3 getLocation() const;

    void lookAt(glm::vec3 position, glm::vec3 up);
    void lookIn(glm::vec3 direction, glm::vec3 up);
    void move(glm::vec3 translation);
    void setTransform(glm::mat4 transform);
    void updateAr(float newAr);
    void setPerspectiveProjection(float fovY, float ar, float near, float far);
    void setView(glm::vec3 position, glm::vec3 direction, glm::vec3 up);
    void setView(glm::mat4 transform);

    static float calculateAspectRatio(uint32_t width, uint32_t height);

private:
    glm::mat4 view{1.f};
    glm::mat4 projection{0.f};
    glm::vec3 position{1.f};
    glm::vec3 direction{1.f};
    glm::vec3 up{1.f};
    float fovY{0.f};
};
