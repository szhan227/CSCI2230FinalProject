#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/transform.hpp"

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent),
    m_lightDir(0, 0.6f, -1.f, 0),
    m_ka(0.1),
    m_kd(0.8),
    m_ks(1),
    m_shininess(15),
    m_angleX(6),
    m_angleY(0),
    m_zoom(2)
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

    // rebuildMatrices();
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    //TODO: delete any FBO, VBO, VAO, etc. you created here

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
    m_w = size().width() * m_devicePixelRatio;
    m_h = size().height() * m_devicePixelRatio;

    //TODO: Call individual initialize_{mountain, terrain, water, sky} functions here
    initSky();
    initWater();
    initTerrain();
    initMountain();

    // set camera
    SceneCameraData init_camera;
    init_camera.pos = glm::vec4(-6.0, 4.0, 4.0, 1.0);
    init_camera.look = glm::vec4(6.0, -4.0, -4.0, 0.0);
    init_camera.up = glm::vec4(0.0, 1.0, 0.0, 0.0);
    init_camera.heightAngle = glm::radians(30.f);
    m_camera.set_Camera(init_camera);
}

void Realtime::paintGL() {
    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjMatrix(width(), height(), 0.1, 1000.f);
    //TODO: Call individual paint_{mountain, terrain, water, sky} functions here
    glViewport(0, 0, m_w, m_h);
    glEnable(GL_CLIP_DISTANCE0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawWater();
    drawSky();
    drawTerrain();
    drawMountain(glm::vec4(0.f,-1.f,0.f,10000.f));
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_w = size().width() * m_devicePixelRatio;
    m_h = size().height() * m_devicePixelRatio;
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

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        float theta_x = float(deltaX) / float(width());
        float theta_y = float(deltaY) / float(height());
        glm::mat3 R_x = glm::mat3{
            cos(theta_x), 0.f, -sin(theta_x),
            0.f         , 1.f,  0.f         ,
            sin(theta_x), 0.f,  cos(theta_x)
        };

        glm::vec3 camera_right = m_camera.get_right();
        glm::mat3 nnt = glm::outerProduct(camera_right, camera_right);
        glm::mat3 N = glm::mat3{
            0.f           , -camera_right.z,  camera_right.y,
            camera_right.z,  0.f           , -camera_right.x,
            -camera_right.y,  camera_right.x,  0.f
        };
        glm::mat3 R_right = glm::cos(theta_y) * glm::mat3(1.f)
                            + (1.f - glm::cos(theta_y)) * nnt
                            + glm::sin(theta_y) * N;

        m_camera.rot(R_x * R_right);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    if(m_keyMap[Qt::Key_W] == true) m_camera.W(deltaTime);
    if(m_keyMap[Qt::Key_S] == true) m_camera.S(deltaTime);
    if(m_keyMap[Qt::Key_A] == true) m_camera.A(deltaTime);
    if(m_keyMap[Qt::Key_D] == true) m_camera.D(deltaTime);
    if(m_keyMap[Qt::Key_Space] == true) m_camera.space(deltaTime);
    if(m_keyMap[Qt::Key_Control] == true) m_camera.control(deltaTime);

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

// ================== Individual Initialization Functions
void Realtime::initSky() {
    //TODO
    m_skyShader = ShaderLoader::createShaderProgram(":/resources/shaders/sky.vert", ":/resources/shaders/sky.frag");

    glGenBuffers(1, &m_sky_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sky_vbo);

    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 50.0f));
    m_sky_model = scaleMatrix;


    m_sky = Sphere(10, 20).getSphereData();
    glBufferData(GL_ARRAY_BUFFER,m_sky.size() * sizeof(GLfloat),m_sky.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_sky_vao);
    glBindVertexArray(m_sky_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

#define ATTACHMENT_NUM 2
const GLenum attachments[ATTACHMENT_NUM] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
};
void Realtime::initWater() {
    m_water_model = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));
    // Create Water Surface
    glGenBuffers(1, &m_water_vbo);
    glGenVertexArrays(1, &m_water_vao);
    m_water.makeSurf();
    m_water.set_gl(m_water_vbo, m_water_vao);
    m_waterShader = ShaderLoader::createShaderProgram(":/resources/shaders/water.vert", ":/resources/shaders/water.frag");
    glUseProgram(m_waterShader);
    glUniform1i(glGetUniformLocation(m_waterShader, "textureSampler1"), 0);
    glUniform1i(glGetUniformLocation(m_waterShader, "textureSampler2"), 1);
    glUniform1i(glGetUniformLocation(m_waterShader, "textureSampler_dudv"), 2);
    glUniform1i(glGetUniformLocation(m_waterShader, "textureSampler_normal"), 3);
    glUseProgram(0);
    //////////////////////////

    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_water_fbo.m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_water_fbo.m_fbo);
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(ATTACHMENT_NUM, m_water_fbo.m_fbo_texture);
    for(int i=0; i<ATTACHMENT_NUM; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_water_fbo.m_fbo_texture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, m_water_fbo.m_fbo_texture[i], 0);
    }

    // load dudv map texture
    QString dudv_filepath = QString(":/resources/images/waterDUDV.png");
    if (!m_dudv.load(dudv_filepath))
    {
        std::cout << "The image path is wrong" << std::endl;
        return;
    }
    m_dudv = m_dudv.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &m_dudv_texture);
    glBindTexture(GL_TEXTURE_2D, m_dudv_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_dudv.width(), m_dudv.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_dudv.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // load normal map texture
    QString normal_filepath = QString(":/resources/images/normalMap.png");
    if (!m_normal.load(normal_filepath))
    {
        std::cout << "The image path is wrong" << std::endl;
        return;
    }
    m_normal = m_normal.convertToFormat(QImage::Format_RGBA8888).mirrored();
    glGenTextures(1, &m_normal_texture);
    glBindTexture(GL_TEXTURE_2D, m_normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_normal.width(), m_normal.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_normal.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_water_fbo.m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_water_fbo.m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_w, m_h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_water_fbo.m_fbo_renderbuffer);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Realtime::initTerrain() {
    //TODO
}

void Realtime::initMountain() {
    m_mountain_shader = ShaderLoader::createShaderProgram(":/resources/shaders/mountain.vert", ":/resources/shaders/mountain.frag");

    glGenBuffers(1, &m_mountain_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_mountain_vbo);


    glGenTextures(1, &m_mountain_rock_texture);
    glBindTexture(GL_TEXTURE_2D, m_mountain_rock_texture);

    //    glGenTextures(1, &m_mountain_grass_texture);
    //    glBindTexture(GL_TEXTURE_2D, m_mountain_grass_texture);

    // Load and set up the texture (you can put this in a separate function if needed)
    QImage rockImage(":/resources/images/rock3.jpg");
    rockImage = rockImage.convertToFormat(QImage::Format_RGBA8888);

    if (rockImage.isNull()) {
        qDebug() << "Failed to load image.";
    } else {
        qDebug() << "Image loaded successfully. Width:" << rockImage.width() << "Height:" << rockImage.height();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rockImage.width(), rockImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, rockImage.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);



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

// ================== Individual Draw Functions
void Realtime::drawSky() {
    //TODO
    glBindVertexArray(m_sky_vao);

    glUseProgram(m_skyShader);

    glUniformMatrix4fv(glGetUniformLocation(m_skyShader, "model"), 1, GL_FALSE, &m_sky_model[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(m_skyShader, "view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_skyShader, "projection"), 1, GL_FALSE, &m_proj[0][0]);

    sun_time += sun_speed;
    float angle = sun_time * 2.0f * M_PI - M_PI;
    float x = -std::sin(angle);
    float y = std::cos(angle);
    m_lightDir = glm::vec4(x, y * 0.3 + 0.2, -1, 0.0);
    glUniform4fv(glGetUniformLocation(m_skyShader, "lightDir"), 1, glm::value_ptr(m_lightDir));

    glUniform4fv(glGetUniformLocation(m_skyShader, "cameraPosition"), 1, &m_camera.get_eye()[0]);

    glDrawArrays(GL_TRIANGLES, 0, m_sky.size() / 3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::drawWater() {
    ////////////////////////////////////////////////////////
    /// Reflaction
    glBindFramebuffer(GL_FRAMEBUFFER, m_water_fbo.m_fbo);
    glDrawBuffers(1, &attachments[0]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMountain(glm::vec4(0.f,-1.f,0.f,0.f));
    ////////////////////////////////////////////////////////
    /// Reflection
    glDrawBuffers(1, &attachments[1]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set virtual camera
    m_camera.flip();
    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjMatrix(width(), height(), 0.1, 1000.f);
//    drawMountain(glm::vec4(0.f,1.f,0.f,0.f));
    drawSky();
    // recover camera
    m_camera.flip();
    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjMatrix(width(), height(), 0.1, 1000.f);
    ////////////////////////////////////////////////////////
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_water_fbo.m_fbo_texture[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_water_fbo.m_fbo_texture[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_dudv_texture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_normal_texture);

    glBindVertexArray(m_water_vao);
    glUseProgram(m_waterShader);
    glUniformMatrix4fv(glGetUniformLocation(m_waterShader, "m_Matrix"), 1, GL_FALSE, &m_water_model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_waterShader, "v_Matrix"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_waterShader, "proj_Matrix"), 1, GL_FALSE, &m_proj[0][0]);
    glUniform4fv(glGetUniformLocation(m_waterShader, "camera_pos"), 1, &(m_camera.get_eye()[0]));
    glUniform4fv(glGetUniformLocation(m_waterShader, "myL.dir"), 1, &(m_lightDir[0]));

    glm::vec4 tmp_light_color = glm::vec4(1, 237.f/255.f, 219.f/255.f, 1);
    glUniform4fv(glGetUniformLocation(m_waterShader, "myL.color"), 1, &(tmp_light_color[0]));
    // movement
    water_time = std::modf(water_time+water_speed, &tmp_empty);
    glUniform1f(glGetUniformLocation(m_waterShader, "time_step"), water_time);
    glDrawArrays(GL_TRIANGLES, 0, m_water.size());
    // Unbind vao and shader
    glBindVertexArray(0);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Realtime::drawTerrain() {
    //TODO
}

void Realtime::drawMountain(glm::vec4 plane) {
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(m_mountain_vao);

    glUseProgram(m_mountain_shader);


    // Bind the texture to a texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_mountain_rock_texture);  // Use the texture created during initialization

    // Set the texture unit to the uniform in the shader
    GLuint rockTextureLocation = glGetUniformLocation(m_mountain_shader, "rockSampler");
    glUniform1i(rockTextureLocation, 0); // 0 corresponds to GL_TEXTURE0

    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "model"), 1, GL_FALSE, &m_sky_model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "projMatrix"), 1, GL_FALSE, &m_proj[0][0]);
    glm::mat4 mvMatrix = m_view * m_mountain_model_matrix;
    glUniformMatrix4fv(glGetUniformLocation(m_mountain_shader, "mvMatrix"), 1, GL_FALSE, &mvMatrix[0][0]);
    glUniform1i(glGetUniformLocation(m_mountain_shader, "wireshade"), false);

    glUniform1f(glGetUniformLocation(m_mountain_shader, "ka"), m_ka);

    glUniform4fv(glGetUniformLocation(m_mountain_shader, "lightDir"), 1, glm::value_ptr(m_lightDir));
    glUniform1f(glGetUniformLocation(m_mountain_shader, "kd"), m_kd);

    glm::mat4 invView = glm::inverse(m_view);
    int res = m_mountain_generator.getResolution();

    glUniform1f(glGetUniformLocation(m_mountain_shader, "ks"), m_ks);
    glUniform1f(glGetUniformLocation(m_mountain_shader, "shininess"), m_shininess);
    glUniform4fv(glGetUniformLocation(m_mountain_shader, "cameraPosition"), 1, glm::value_ptr(invView[3]));

    // add clip height for water rendering
    glUniform4fv(glGetUniformLocation(m_mountain_shader, "plane"), 1, &(plane[0]));

    glDrawArrays(GL_TRIANGLES, 0, res * res * 6);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Realtime::rebuildMatrices() {
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
