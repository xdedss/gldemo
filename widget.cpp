#include "widget.h"
#include <stdio.h>
#include <iostream>


Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fixedUpdate()));
    timer->start(20);
}

Widget::~Widget()
{

}


static const Vertex sg_vertexes[] = {
    Vertex(QVector3D(0.00f,  0.75f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)),
    Vertex(QVector3D(0.75f, -0.75f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)),
    Vertex(QVector3D(-0.75f, -0.75f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)),
    Vertex(QVector3D(0.00f,  0.75f, 1.0f), QVector3D(1.0f, 1.0f, 0.0f)),
    Vertex(QVector3D(0.75f, -0.75f, 2.0f), QVector3D(0.0f, 1.0f, 1.0f)),
    Vertex(QVector3D(-0.75f, -0.75f, 0.0f), QVector3D(1.0f, 0.0f, 1.0f)),
};
static const Vertex sg_vertexes_2[] = {
    Vertex(QVector3D(-1.00f,  -0.75f, -1.0f), QVector3D(1.0f, 0.0f, 0.0f)),
    Vertex(QVector3D(-0.75f, 0.75f, -1.0f), QVector3D(0.0f, 1.0f, 0.0f)),
    Vertex(QVector3D(0.75f, 0.75f, -1.0f), QVector3D(0.0f, 0.0f, 1.0f)),
    Vertex(QVector3D(0.00f,  -0.75f, 1.0f), QVector3D(1.0f, 1.0f, 0.0f)),
    Vertex(QVector3D(0.75f, 0.75f, 2.0f), QVector3D(0.0f, 1.0f, 1.0f)),
    Vertex(QVector3D(-0.75f, 0.75f, 0.0f), QVector3D(1.0f, 0.0f, 1.0f)),
};

//鼠标 点击
void Widget::mousePressEvent(QMouseEvent * e)
{
    mouseDown = true;
    //获取点击的下标
    //qDebug() << e->x() << ":" << e->y();
    if (e->button() == Qt::LeftButton)
    {
        qDebug() << "左键" << e->pos();
        
    }
    else if (e->button() == Qt::RightButton)
    {
        qDebug() << "右键" << e->pos();
    }
}
//鼠标 移动
void Widget::mouseMoveEvent(QMouseEvent * e)
{
    //qDebug() << "move" << e->x() << ":" << e->y();
    mousex = e->x(); mousey = e->y();
}
//鼠标 松开
void Widget::mouseReleaseEvent(QMouseEvent * e)
{
    mouseDown = false;
    //qDebug() << "Release" << e->x() << ":" << e->y();
}

QOpenGLShaderProgram* loadShader(const std::string& name) {
    auto res = new QOpenGLShaderProgram();
    res->addShaderFromSourceFile(QOpenGLShader::Vertex, ("./" + name + ".vert").c_str());
    res->addShaderFromSourceFile(QOpenGLShader::Fragment, ("./" + name + ".frag").c_str());
    res->link();
    return res;
}

void Widget::initializeGL()
{
    this->initializeOpenGLFunctions();    //为当前上下文初始化提供OpenGL函数解析
    glEnable(GL_DEPTH_TEST);
    
    m_program = loadShader("foo");


    pointCloud = new PointCloud[2]();
    pointCloud[0].init();
    pointCloud[0].shader = m_program;
    pointCloud[0].vertices = readPly("bun000.ply");
    pointCloud[0].applyVertices();
    pointCloud[1].init();
    pointCloud[1].shader = m_program;
    pointCloud[1].vertices = readPly("bun180.ply");
    pointCloud[1].applyVertices();

    m_program->release();


    model = glm::identity<glm::mat4>();
    projection = glm::identity<glm::mat4>();
    view = glm::identity<glm::mat4>();
    camPos = { 0, 0, 3 };
    camUp = { 0, 1, 0 };
    camTarget = { 0, 0, 0 };
}

void Widget::fixedUpdate() {
    //modelAngle += 0.02f;
    if (mouseDown) {
        float dx = mousex - mouselastx;
        float dy = mousey - mouselasty;
        pitch += -dy * 0.02;
        yaw += -dx * 0.02;
    }
    mouselastx = mousex;
    mouselasty = mousey;
    update();
}

void Widget::resizeGL(int w, int h)
{
    glViewport(0.0f,0.0f,w,h);    //调整视口
    screenWidth = w;
    screenHeight = h;
}

void Widget::paintGL()
{

    glClearColor(0.2f, 0.5f, 0.9f, 1.0f);    //清屏
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    //清除颜色缓冲

    auto camRotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * glm::angleAxis(pitch, glm::vec3(1, 0, 0));

    model = glm::rotate(glm::identity<glm::mat4>(), modelAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    projection = glm::perspective(glm::radians(45.0f), screenWidth / (float)screenHeight, 0.1f, 100.0f);
    
    camPos = camRotation * glm::vec3(0, 0, 5);
    camUp = camRotation * glm::vec3(0, 1, 0);
    view = glm::lookAt(camPos, camTarget, camUp);
    //view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -3.0f));


    // Render using our shader
    m_program->bind();
    {
        glUniformMatrix4fv(m_program->uniformLocation("MAT_MODEL"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(m_program->uniformLocation("MAT_PROJ"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(m_program->uniformLocation("MAT_VIEW"), 1, GL_FALSE, glm::value_ptr(view));

        pointCloud[0].transform = model;
        pointCloud[0].render();

        pointCloud[1].transform = model;
        pointCloud[1].render();
    }
    m_program->release();
    //glUseProgram(shaderProgram);
    //glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

}
