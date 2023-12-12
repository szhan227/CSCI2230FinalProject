#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "camera/camera.h"
#include "utils/shaderloader.h"
#include "shape/sphere.h"
#include "glm/gtx/transform.hpp"

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "shape/water.h"
struct water_fbo
{
    // 0: refraction
    // 1: reflection
    GLuint m_fbo;
    GLuint m_fbo_texture[2];
    GLuint m_fbo_renderbuffer;
};

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    const Camera& getCamera() const;
    glm::mat3 makeRodrigues(glm::vec3 axis, float angle);
    void rebuildMatrices();

    // Initialization functions
    void initSky();
    void initWater();
    void initTerrain();
    void initMountain();
    
    // Draw functions
    void drawSky();
    void drawWater();
    void drawTerrain();
    void drawMountain();

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;
    float m_h=640.f;
    float m_w=640.f;
    GLuint m_defaultFBO=2;

    // Camera
    Camera m_camera;

    //TODO: add FBO, VAO, VBO, variables for each landscape component

    // Sky variables
    GLuint m_skyShader;
    GLuint m_sky_vbo;
    GLuint m_sky_vao;
    std::vector<float> m_sky;
    glm::mat4 m_sky_model;
    float sun_speed = 0.001;
    float sun_time = 0.f;

    // Water variablesz
    glm::mat4 m_water_model;
    GLuint m_waterShader;
    GLuint m_water_vbo;
    GLuint m_water_vao;
    water_fbo m_water_fbo;
    Water m_water;
    QImage m_dudv;
    QImage m_normal;
    GLuint m_dudv_texture;
    GLuint m_normal_texture;
    float water_speed = 0.001;
    float water_time = 0.f;
    float tmp_empty;
    // Terrain variables

    // Mountain variables

    // Variables that are shared across all landscapes (Need to be discussed)
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    glm::vec4 m_lightDir; // one dummy light for now 

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;
};
