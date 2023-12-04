#include "glrenderer.h"
#include "utils/shape.h"

#include <QCoreApplication>
#include "src/shaderloader.h"

#include <QMouseEvent>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/transform.hpp"

GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_lightDir(-0.3,-1.0,-0.7, 0),
      m_ka(0.1),
      m_kd(0.8),
      m_ks(1),
      m_shininess(15),
      m_angleX(6),
      m_angleY(0),
      m_zoom(2)
{
    rebuildMatrices();
}

GLRenderer::~GLRenderer()
{
    makeCurrent();
    doneCurrent();
}

void GLRenderer::initializeGL()
{
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) fprintf(stderr, "Error while initializing GLEW: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Successfully initialized GLEW %s\n", glewGetString(GLEW_VERSION));

    glClearColor(0,0,0,1);

    glEnable(GL_DEPTH_TEST);
    m_skyShader = ShaderLoader::createShaderProgram(":/resources/shaders/sky.vert", ":/resources/shaders/sky.frag");

    // Create Sky Sphere
    glGenBuffers(1, &m_sky_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sky_vbo);
    m_skySphere = Sphere(10, 20).getSphereData();
    glBufferData(GL_ARRAY_BUFFER,m_skySphere.size() * sizeof(GLfloat),m_skySphere.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_sky_vao);
    glBindVertexArray(m_sky_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);


    // Create Mountains
    initializeMountain();
}

void GLRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(m_sky_vao);

    glUseProgram(m_skyShader);

    glUniformMatrix4fv(glGetUniformLocation(m_skyShader, "model"), 1, GL_FALSE, &m_sky_model[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(m_skyShader, "view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_skyShader, "projection"), 1, GL_FALSE, &m_proj[0][0]);

    glUniform1f(glGetUniformLocation(m_skyShader, "ka"), m_ka);

    glUniform4fv(glGetUniformLocation(m_skyShader, "lightDir"), 1, glm::value_ptr(m_lightDir));
    glUniform1f(glGetUniformLocation(m_skyShader, "kd"), m_kd);

    glm::mat4 invView = glm::inverse(m_view);

    glUniform1f(glGetUniformLocation(m_skyShader, "ks"), m_ks);
    glUniform1f(glGetUniformLocation(m_skyShader, "shininess"), m_shininess);
    glUniform4fv(glGetUniformLocation(m_skyShader, "cameraPosition"), 1, glm::value_ptr(invView[3]));


    glDrawArrays(GL_TRIANGLES, 0, m_skySphere.size() / 3);
    glBindVertexArray(0);

    glUseProgram(0);

    // paint the mountain
    paintMountain();
}


// ============= helper functions for mountain ==============

// --- call this in initializeGL to initialize mountain related components
void GLRenderer::initializeMountain(){
    m_mountain_shader = ShaderLoader::createShaderProgram(":/resources/shaders/mountain.vert", ":/resources/shaders/mountain.frag");

    glGenBuffers(1, &m_mountain_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_mountain_vbo);

    m_mountain_verts = m_mountain_generator.generateMountain();

    glBufferData(GL_ARRAY_BUFFER,m_mountain_verts.size() * sizeof(GLfloat),m_mountain_verts.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_mountain_vao);
    glBindVertexArray(m_mountain_vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
                          nullptr);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat),
                          reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

// --- call this in paintGL to paint mountain related components
void GLRenderer::paintMountain(){
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(m_mountain_vao);

    glUseProgram(m_mountain_shader);

    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "model"), 1, GL_FALSE, &m_sky_model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);
    glm::mat4 mvMatrix = m_view * m_mountain_model_matrix;
    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);

    glUniform1f(glGetUniformLocation(m_mountain_shader, "ka"), m_ka);

    glUniform4fv(glGetUniformLocation(m_mountain_shader, "lightDir"), 1, glm::value_ptr(m_lightDir));
    glUniform1f(glGetUniformLocation(m_mountain_shader, "kd"), m_kd);

    glm::mat4 invView = glm::inverse(m_view);
    int res = m_mountain_generator.getResolution();

    glUniform1f(glGetUniformLocation(m_mountain_shader, "ks"), m_ks);
    glUniform1f(glGetUniformLocation(m_mountain_shader, "shininess"), m_shininess);
    glUniform4fv(glGetUniformLocation(m_mountain_shader, "cameraPosition"), 1, glm::value_ptr(invView[3]));


    glDrawArrays(GL_TRIANGLES, 0, res * res * 6);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);
}

// ============= end of functions for mountain ==============

// ================== Other stencil code

void GLRenderer::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::radians(45.0),1.0 * w / h,0.01,1000.0);
}

void GLRenderer::mousePressEvent(QMouseEvent *event) {
    // Set initial mouse position
    m_prevMousePos = event->pos();
}

void GLRenderer::mouseMoveEvent(QMouseEvent *event) {
    // Update angle member variables based on event parameters
    m_angleX += 10 * (event->position().x() - m_prevMousePos.x()) / (float) width();
    m_angleY += 10 * (event->position().y() - m_prevMousePos.y()) / (float) height();
    m_prevMousePos = event->pos();
    rebuildMatrices();
}

void GLRenderer::wheelEvent(QWheelEvent *event) {
    // Update zoom based on event parameter
    m_zoom -= event->angleDelta().y() / 1000.f;
    rebuildMatrices();
}

void GLRenderer::rebuildMatrices() {
    // Update view matrix by rotating eye vector based on x and y angles
    m_view = glm::mat4(1);
    glm::mat4 rot = glm::rotate(glm::radians(-10 * m_angleX),glm::vec3(0,0,1));
    glm::vec3 eye = glm::vec3(2,0,0);
    eye = glm::vec3(rot * glm::vec4(eye,1));

    rot = glm::rotate(glm::radians(-10 * m_angleY),glm::cross(glm::vec3(0,0,1),eye));
    eye = glm::vec3(rot * glm::vec4(eye,1));

    eye = eye * m_zoom;

    m_view = glm::lookAt(eye,glm::vec3(0,0,0),glm::vec3(0,0,1));

    m_proj = glm::perspective(glm::radians(45.0),1.0 * width() / height(),0.01,1000.0);

    update();
}
