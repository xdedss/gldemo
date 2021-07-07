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
    /*
    if (LeftMouseDown) {
        windowX = this->x();
        windowY = this->y();
        wheeldelta = (float)wheel->delta();
    }
    else {
        distance *= exp(0.001 * wheel->delta());
    }*/
    wheeldelta = (float)wheel->delta();
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
    else if (e->button() == Qt::LeftButton) {
        LeftMouseDown = true;
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
    LeftMouseDown = false;
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

    //如果鼠标按下，记录鼠标走的方位，进行相机旋转操作 
    if (RightMouseDown) {
        float dx = mousex - mouselastx;
        float dy = mousey - mouselasty;
        pitch += -dy * 1.5 / sqrt(screenWidth * screenHeight);//与屏幕大小有关的因子
        yaw += -dx * 1.5 / sqrt(screenWidth * screenHeight);
        
    }
    
    // 键盘平移与鼠标滚轮缩放
    if (camDx != 0 || camDy != 0 || (wheeldelta)) {
        glm::vec3 z = glm::normalize(camPos - camTarget);
        glm::vec3 x = glm::normalize(glm::cross(camUp, z));//叉乘确定Y轴
        glm::vec3 y = glm::normalize(camUp);
        if (wheeldelta) {
            qDebug() << mousex<<windowX;
            camTarget += -0.03f * (mousex  - (int)screenWidth / 2) * ((float)exp(0.001 * wheeldelta)-1)* (float)(log10(1 + 0.3*distance)+0.1*distance) * x ;
            camTarget += 0.03f * (mousey - (int)screenHeight/2) * ((float)exp(0.001 * wheeldelta)-1) * (float)(log10(1 + 0.3*distance)+0.1*distance) * y;
            distance *= exp(0.001 * wheeldelta);
            wheeldelta = 0.0f;
            qDebug() << camTarget.x << camTarget.y << camTarget.z;
        }
        else {
            camTarget += (float)camDx * 0.05f * x * distance;
            camTarget += (float)camDy * 0.05f * y * distance;
            qDebug() << camTarget.x << camTarget.y << camTarget.z;
        }
    }

    // 更新lastx lasty
    mouselastx = mousex;
    mouselasty = mousey;
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
    
    // 加载shader
    m_program = loadShader("foo");

    // 加载两个模型
    pointClouds.push_back(new PointCloud());
    pointClouds[0]->init();
    pointClouds[0]->shader = m_program;
    pointClouds[0]->vertices = readPly("bun000.ply");
    pointClouds[0]->applyVertices();
    pointClouds[0]->transform = glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * 10.0f);

    pointClouds.push_back(new PointCloud());
    pointClouds[1]->init();
    pointClouds[1]->shader = m_program;
    pointClouds[1]->vertices = readTxt("uwo.txt");
    pointClouds[1]->applyVertices();
    pointClouds[1]->transform = glm::rotate(
        glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * 0.1f), 
        3.5f, { 1.0f, 0.0f, 0.0f });

    m_program->release();

    // 初始化相机位置姿态
    projection = glm::identity<glm::mat4>();
    view = glm::identity<glm::mat4>();
    camPos = { 0, 0, 3 };
    camUp = { 0, 1, 0 };
    camTarget = { 0.0, 0.0, 0.0 };
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

    //透视(视锥上下面之间的夹角，宽高比，即视窗的宽/高，近截面、远截面的深度)
    projection = glm::perspective(glm::radians(45.0f), screenWidth / (float)screenHeight, 0.01f, 300.0f);
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

        for (auto pointCloud : pointClouds) {
            pointCloud->render();
        }
    }
    m_program->release();
    //glUseProgram(shaderProgram);
    //glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

}
