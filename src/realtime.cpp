#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::sceneChanged() {

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

glm::mat3 Realtime::makeRodrigues(glm::vec3 axis, float angle) {
    glm::mat3 rodrigue = glm::mat3(1);

    rodrigue[0][0] = glm::cos(angle) + (1 - glm::cos(angle)) * axis.x * axis.x;
    rodrigue[0][1] = (1 - glm::cos(angle)) * axis.x * axis.y + glm::sin(angle) * axis.z;
    rodrigue[0][2] = (1 - glm::cos(angle)) * axis.x * axis.z - glm::sin(angle) * axis.y;
    rodrigue[1][0] = (1 - glm::cos(angle)) * axis.x * axis.y - glm::sin(angle) * axis.z;
    rodrigue[1][1] = glm::cos(angle) + (1 - glm::cos(angle)) * axis.y * axis.y;
    rodrigue[1][2] = (1 - glm::cos(angle)) * axis.y * axis.z + glm::sin(angle) * axis.x;
    rodrigue[2][0] = (1 - glm::cos(angle)) * axis.x * axis.z + glm::sin(angle) * axis.y;
    rodrigue[2][1] = (1 - glm::cos(angle)) * axis.y * axis.z - glm::sin(angle) * axis.x;
    rodrigue[2][2] = glm::cos(angle) + (1 - glm::cos(angle)) * axis.z * axis.z;

    return rodrigue;
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        float rotation = 0.01 * deltaX;
        glm::mat3 rodrigue = makeRodrigues(glm::vec3(0,1,0), rotation);
        camera.setLook(glm::vec4(rodrigue * glm::vec3(camera.getLook()), 0.));

        rotation = 0.01 * deltaY;
        rodrigue = makeRodrigues(glm::normalize(glm::cross(glm::vec3(camera.getLook()), glm::vec3(camera.getUp()))), rotation);
        camera.setLook(glm::vec4(rodrigue * glm::vec3(camera.getLook()), 0.));

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float translation = 5. * deltaTime;

    // update camera's position as well as the view matrix
    if (m_keyMap[Qt::Key_W]) {
        camera.setPos(camera.getPos() + translation * glm::normalize(camera.getLook()));
    }

    if (m_keyMap[Qt::Key_S]) {
        camera.setPos(camera.getPos() - translation * glm::normalize(camera.getLook()));
    }

    if (m_keyMap[Qt::Key_A]) {
        glm::vec3 u = glm::normalize(glm::cross(glm::vec3(camera.getUp()), glm::vec3(camera.getLook())));
        camera.setPos(camera.getPos() + glm::vec4(translation * u, 0.));
    }

    if (m_keyMap[Qt::Key_D]) {
        glm::vec3 u = glm::normalize(glm::cross(glm::vec3(camera.getUp()), glm::vec3(camera.getLook())));
        camera.setPos(camera.getPos() - glm::vec4(translation * u, 0.));
    }

    if (m_keyMap[Qt::Key_Control]) {
        camera.setPos(camera.getPos() + translation * glm::vec4(0, -1, 0, 0));
    }

    if (m_keyMap[Qt::Key_Space]) {
        camera.setPos(camera.getPos() + translation * glm::vec4(0, 1, 0, 0));
    }

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}