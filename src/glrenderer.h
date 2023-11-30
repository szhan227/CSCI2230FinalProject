#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "GL/glew.h" // Must always be first include
#include <QOpenGLWidget>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLRenderer : public QOpenGLWidget
{
public:
    GLRenderer(QWidget *parent = nullptr);
    ~GLRenderer();

protected:
    void initializeGL()                  override; // Called once at the start of the program
    void paintGL()                       override; // Called every frame in a loop
    void resizeGL(int width, int height) override; // Called when window size changes

    void mousePressEvent(QMouseEvent *e) override; // Used for camera movement
    void mouseMoveEvent(QMouseEvent *e)  override; // Used for camera movement
    void wheelEvent(QWheelEvent *e)      override; // Used for camera movement
    void rebuildMatrices();                        // Used for camera movement

private:
    GLuint m_skyShader;
    GLuint m_sky_vbo;
    GLuint m_sky_vao;
    std::vector<float> m_skySphere;

    glm::mat4 m_sky_model = glm::scale(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 500.0f));

    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    glm::vec4 m_lightDir;

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;
};
