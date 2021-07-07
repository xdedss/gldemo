#include "widget.h"
#include <stdio.h>
#include <iostream>


Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(100, 100);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fixedUpdate()));
    timer->start(20);
}

Widget::~Widget()
{

}

//滚轮
void Widget::wheelEvent(QWheelEvent* wheel) {
    distance *= exp(0.002 * wheel->delta());
}
//鼠标 点击
void Widget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::RightButton) {
        RightMouseDown = true;
        qDebug() << "右键" << e->pos();
    }
    else if (e->button() == Qt::MidButton) {
        MidMouseDown = true;
    }
    //获取点击的下标
    //qDebug() << e->x() << ":" << e->y();
}
//鼠标 移动
void Widget::mouseMoveEvent(QMouseEvent* e)
{
    //qDebug() << "move" << e->x() << ":" << e->y();
    mousex = e->x(); mousey = e->y();
}
//鼠标 松开
void Widget::mouseReleaseEvent(QMouseEvent* e)
{
    RightMouseDown = false;
    MidMouseDown = false;
    //qDebug() << "Release" << e->x() << ":" << e->y();
}
//键盘 事件
void Widget::keyPressEvent(QKeyEvent* key) {
    switch (key->key()) {
    case Qt::Key_W:
        camDy = 1;
        break;
    case Qt::Key_A:
        camDx = -1;
        break;
    case Qt::Key_S:
        camDy = -1;
        break;
    case Qt::Key_D:
        camDx = 1;
        break;
    }
}

void Widget::keyReleaseEvent(QKeyEvent* key) {
    if (key->key() == Qt::Key_A || key->key() == Qt::Key_W || key->key() == Qt::Key_S || key->key() == Qt::Key_D)
    {
        camDx = 0;
        camDy = 0;
    }
}

void Widget::fixedUpdate() {
    //modelAngle += 0.02f;//每次旋转0.02  //物体自转
    if (RightMouseDown) {
        float dx = mousex - mouselastx;
        float dy = mousey - mouselasty;
        pitch += -dy * 1.5 / sqrt(screenWidth * screenHeight);//与屏幕大小有关的因子
        yaw += -dx * 1.5 / sqrt(screenWidth * screenHeight);
    }//如果鼠标按下，记录鼠标走的方位，进行相机旋转操作
    mouselastx = mousex;
    mouselasty = mousey;
    if (camDx != 0 || camDy != 0) {
        glm::vec3 z = glm::normalize(camPos - camTarget);
        glm::vec3 x = glm::normalize(glm::cross(camUp, z));//叉乘确定Y轴
        glm::vec3 y = glm::normalize(camUp);
        camTarget += (float)camDx * 0.05f * x * distance;
        camTarget += (float)camDy * 0.05f * y * distance;
    }
    update();//重新渲染
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
    pointCloud[0].transform = glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * 10.0f);

    pointCloud[1].init();
    pointCloud[1].shader = m_program;
    pointCloud[1].vertices = readTxt("uwo.txt");
    pointCloud[1].applyVertices();
    pointCloud[1].transform = glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * 0.1f);

    m_program->release();


    model = glm::identity<glm::mat4>();
    projection = glm::identity<glm::mat4>();
    view = glm::identity<glm::mat4>();
    camPos = { 0, 0, 3 };
    camUp = { 0, 1, 0 };
    camTarget = { 0, 0, 0 };
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

    //模型旋转
    //rotate(起始矩阵，旋转角度，旋转轴)
    model = glm::rotate(glm::identity<glm::mat4>(), modelAngle, glm::vec3(0.0f, 0.0f, 1.0f));
    //透视(视锥上下面之间的夹角，宽高比，即视窗的宽/高，近截面、远截面的深度)
    projection = glm::perspective(glm::radians(45.0f), screenWidth / (float)screenHeight, 0.2f, 200.0f);
    //相机位置更新
    //相机旋转矩阵，pos旋转后 = camRotation * pos旋转前
    auto camRotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * glm::angleAxis(pitch, glm::vec3(1, 0, 0));
    camPos = camRotation * glm::vec3(0, 0, distance) + camTarget;////可以实现绕不同点转 camTarget 表示相机拍摄视角的目标点
    camUp = camRotation * glm::vec3(0, 1, 0);//相机y轴（相机正上方）的指向。。。默认相机永远在被观察物体的z轴上
    view = glm::lookAt(camPos, camTarget, camUp);//相机在pos的位置上，看 camtarget ，以camUP的角度（这里可以实现目标点在视线的中心）

    //开始渲染
    // 开启着色器
    m_program->bind();
    {
        glUniformMatrix4fv(m_program->uniformLocation("MAT_PROJ"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(m_program->uniformLocation("MAT_VIEW"), 1, GL_FALSE, glm::value_ptr(view));

        pointCloud[0].render();

        pointCloud[1].render();
    }
    m_program->release();
    //glUseProgram(shaderProgram);
    //glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

}
