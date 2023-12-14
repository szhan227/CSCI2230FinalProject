#ifndef WATER_H
#define WATER_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Water
{
public:
    void makeSurf();
    void set_gl(GLuint m_vbo, GLuint m_vao);
    int size() { return m_vertexData.size() / 6; }
private:
    std::vector<float> m_vertexData;
    void pushVec3(glm::vec3 vec);
};

#endif // WATER_H
