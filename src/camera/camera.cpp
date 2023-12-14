#include "camera.h"
#include <iostream>
#include "glm/gtx/transform.hpp"


void Camera::flip()
{
    pos.y *= -1;
    dir.y *= -1;
    right = glm::normalize(glm::cross(wup, dir));
    up = glm::normalize(glm::cross(dir, right));
}

void Camera::set_Camera(SceneCameraData camera) {
    std::cout << "Set camera paramters" << std::endl;
    pos = glm::vec3(camera.pos);
    dir = glm::normalize(glm::vec3(camera.look));
    up = glm::normalize(glm::vec3(camera.up));
    right = glm::normalize(glm::cross(up, dir));
    up = glm::normalize(glm::cross(dir, right));
    heightAngle = camera.heightAngle;
}

glm::mat4 getTranslationMatrix(float dx, float dy, float dz) {
    glm::vec4 m0 = glm::vec4(1.f,0.f,0.f,0.f);
    glm::vec4 m1 = glm::vec4(0.f,1.f,0.f,0.f);
    glm::vec4 m2 = glm::vec4(0.f,0.f,1.f,0.f);
    glm::vec4 m3 = glm::vec4(dx,dy,dz,1.f);
    glm::mat4 T = glm::mat4(m0,m1,m2,m3);
    return T;
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 w = - glm::normalize(dir);
    glm::vec3 v = glm::normalize(up-glm::dot(up,w)*w);
    glm::vec3 u = glm::cross(v,w);
    glm::vec4 R0 = glm::vec4(u[0],v[0],w[0],0.0f);
    glm::vec4 R1 = glm::vec4(u[1],v[1],w[1],0.0f);
    glm::vec4 R2 = glm::vec4(u[2],v[2],w[2],0.0f);
    glm::vec4 R3 = glm::vec4(0.f,0.f,0.f,1.f);
    return glm::mat4(R0,R1,R2,R3)*getTranslationMatrix(-pos[0],-pos[1],-pos[2]);
}

glm::mat4 Camera::getProjMatrix(int width, int height, float near_z, float far_z) const
{
    float view_height = 2.f * tan(heightAngle/2) ;
    float view_width = getAspectRatio(width, height) * view_height;
    float weightAngle = 2 * atan(view_width/2);
    float c = - near_z / far_z;

    glm::mat4 S = glm::mat4{1/(far_z * tan(weightAngle/2)), 0.f, 0.f, 0.f,
                            0.f, 1/(far_z * tan(heightAngle/2)), 0.f, 0.f,
                            0.f, 0.f, 1/far_z, 0.f,
                            0.f, 0.f, 0.f, 1.f};

    glm::mat4 pp =   glm::mat4{1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1/(1+c), -1.f,
        0.f, 0.f, -c/(1+c), 0.f,};

    glm::mat4 remapping =   glm::mat4{1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, -2.f, 0.f,
        0.f, 0.f, -1.f, 1.f,};

    return remapping * pp * S;

}

float Camera::getAspectRatio(int width, int height) const {
    // Optional TODO: implement the getter or make your own design
    return float(width)/float(height);
}

float Camera::getHeightAngle() const {
    // Optional TODO: implement the getter or make your own design
    return heightAngle;
}



void Camera::W(float deltaTime)
{
    pos = pos + dir * step * deltaTime;
}

void Camera::S(float deltaTime)
{
    pos = pos - dir * step * deltaTime;
}


void Camera::A(float deltaTime)
{
    pos = pos + right * step * deltaTime;
}


void Camera::D(float deltaTime)
{
    pos = pos - right * step * deltaTime;
}


void Camera::space(float deltaTime)
{
    pos = pos + wup * step * deltaTime;
}


void Camera::control(float deltaTime)
{
    pos = pos - wup * step * deltaTime;
}

void Camera::rot(glm::mat3 R)
{
    dir = dir * R;
    up = up * R;
    right = right * R;
}



