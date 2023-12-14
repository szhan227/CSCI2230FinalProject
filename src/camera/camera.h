#pragma once

#include "utils/scenedata.h"
// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    void set_Camera(SceneCameraData camera);
    glm::vec4 get_eye() const {return glm::vec4(pos,1.0);}
    glm::vec3 get_dir() const {return dir;}
    glm::vec3 get_right() const {return right;}

    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjMatrix(int width, int height, float near_z, float far_z) const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio(int width, int height) const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // key event
    void W(float deltaTime);
    void S(float deltaTime);
    void A(float deltaTime);
    void D(float deltaTime);
    void space(float deltaTime);
    void control(float deltaTime);
    void rot(glm::mat3 R);

    void flip();

private:
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 wup{0.f,1.f,0.f};
    float step = 5.f;
    float heightAngle;
};
