#pragma once

#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::mat4 getInverseViewMatrix() const;

    glm::mat4 getPerspectiveMatrix() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

    glm::vec4 getPos() const
    {
        return glm::vec4(pos, 1);
    }

    glm::vec4 getLook() const
    {
        return glm::vec4(look, 1);
    }

    glm::vec4 getUp() const
    {
        return glm::vec4(up, 1);
    }

    // setters
    void setWidth(int w) { width = w; }
    void setHeight(int h) { height = h; }
    void setPos(glm::vec3 p) { pos = p; }
    void setLook(glm::vec3 l) { look = l; }
    void setUp(glm::vec3 u) { up = u; }
    void setAspectRatio(float ar) { aspect_ratio = ar; }
    void setHeightAngle(float ha) { height_angle = ha; }
    void setFocalLength(float fl) { focalLength = fl; }
    void setAperture(float a) { aperture = a; }
    void setNear(float n) {near = n; }
    void setFar(float f) {far = f; }

private:
    int width, height;
    glm::vec3 pos, look, up;
    float aspect_ratio, height_angle;
    float focalLength, aperture;
    float near, far;
};