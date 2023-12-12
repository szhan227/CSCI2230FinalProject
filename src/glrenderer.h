#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "GL/glew.h" // Must always be first include
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <QMatrix4x4>
#include "src/mountaingenerator.h"

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

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    void rebuildMatrices();                        // Used for camera movement

private:

    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    GLuint m_skyShader;
    GLuint m_sky_vbo;
    GLuint m_sky_vao;
    std::vector<float> m_skySphere;

    glm::mat4 m_sky_model = glm::scale(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 500.0f));

    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    glm::vec4 m_lightDir;

    glm::vec4 cameraPosV = glm::vec4(0, 0, 0, 1);
    glm::vec4 cameraPosW = glm::vec4(0, 0, 0, 1);

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;


    // ------------------ Mountain Component start -------------------
    int m_xRot = 0;
    int m_yRot = 0;
    int m_zRot = 0;

    glm::mat4 m_mountain_model_matrix = glm::mat4(3.f);

    GLuint m_mountain_shader;
    GLuint  m_mountain_vao;
    GLuint  m_mountain_vbo;
    GLuint m_mountain_rock_texture;
    GLuint m_mountain_grass_texture;

    MountainGenerator m_mountain_generator;
    std::vector<GLfloat> m_mountain_verts;

    void initializeMountain();
    void paintMountain();
//    int m_projMatrixLoc = 0;
//    int m_mvMatrixLoc = 0;
    // ------------------ Mountain Component end ------------------
};
