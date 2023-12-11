#include "water.h"


void Water::makeSurf()
{
    float size = 1.f;
    float h = 0.f;
    m_vertexData.clear();
    glm::vec3 v00{-size/2.0, h, -size/2.0};
    glm::vec3 v10{ size/2.0, h, -size/2.0};
    glm::vec3 v01{-size/2.0, h,  size/2.0};
    glm::vec3 v11{ size/2.0, h,  size/2.0};
    glm::vec3 n{0.f, 1.f, 0.f};

    pushVec3(v00);
    pushVec3(n);
    pushVec3(v01);
    pushVec3(n);
    pushVec3(v10);
    pushVec3(n);

    pushVec3(v10);
    pushVec3(n);
    pushVec3(v01);
    pushVec3(n);
    pushVec3(v11);
    pushVec3(n);
}


void Water::pushVec3(glm::vec3 vec)
{
    m_vertexData.push_back(vec.x);
    m_vertexData.push_back(vec.y);
    m_vertexData.push_back(vec.z);
}


void Water::set_gl(GLuint m_vbo, GLuint m_vao)
{
    if(m_vertexData.size()==0) return;
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(GLfloat), m_vertexData.data(), GL_STATIC_DRAW);
    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(0));
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
