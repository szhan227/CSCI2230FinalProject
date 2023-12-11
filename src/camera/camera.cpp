#include <stdexcept>
#include "camera.h"

glm::mat4 Camera::getViewMatrix() const {
    // Optional TODO: implement the getter or make your own design
    glm::vec3 w = glm::normalize(look);
    for (int i = 0; i < 3; i++) {
        w[i] = -w[i];
    }

    glm::vec3 v = glm::normalize(up - glm::dot(up, w) * w);

    glm::vec3 u = glm::cross(v, w);

    glm::mat4 rotate = glm::mat4(0.f);
    rotate[0][0] = u[0];
    rotate[1][0] = u[1];
    rotate[2][0] = u[2];
    rotate[0][1] = v[0];
    rotate[1][1] = v[1];
    rotate[2][1] = v[2];
    rotate[0][2] = w[0];
    rotate[1][2] = w[1];
    rotate[2][2] = w[2];
    rotate[3][3] = 1.f;

    glm::mat4 translate = glm::mat4(0.f);
    translate[0][0] = 1.f;
    translate[1][1] = 1.f;
    translate[2][2] = 1.f;
    translate[3][3] = 1.f;
    translate[3][0] = -pos[0];
    translate[3][1] = -pos[1];
    translate[3][2] = -pos[2];

    return rotate * translate;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    glm::mat4 view = getViewMatrix();

    return glm::inverse(view);
}

glm::mat4 Camera::getPerspectiveMatrix() const {
    glm::mat4 clip_transform = glm::mat4(1.f);
    clip_transform[2][2] = -2.f;
    clip_transform[3][2] = -1.f;

    glm::mat4 parallel = glm::mat4(1.f);
    parallel[3][3] = 0.f;
    parallel[2][3] = -1.f;
    float c = -near / far;
    parallel[2][2] = 1.f/ (1.f + c);
    parallel[3][2] = -c / (1.f + c);

    glm::mat4 scaling = glm::mat4(1.f);
    scaling[0][0] = 1.f / (far * tan(height_angle * aspect_ratio / 2.f));
    scaling[1][1] = 1.f / (far * tan(height_angle / 2.f));
    scaling[2][2] = 1.f / far;

    return clip_transform * parallel * scaling; 
}

float Camera::getAspectRatio() const {
    // Optional TODO: implement the getter or make your own design
    return aspect_ratio;
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return height_angle;
}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}