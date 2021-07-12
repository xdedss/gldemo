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

OpenGLFunctions * Widget::functions() const
{
    //qDebug << ((intptr_t)this);
    assert(context());
    return context()->versionFunctions<OpenGLFunctions>();
}

//滚轮
void Widget::wheelEvent(QWheelEvent* wheel) {
    wheeldelta = (float)wheel->delta();//记录滚轮转动
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
        qDebug() << "滚轮" << e->pos();
    }
    else if (e->button() == Qt::LeftButton) {
        LeftMouseDown = true;
        qDebug() << "左键" << e->pos();
        //pointClouds[0]->nearestSearch({ 0, 0, 0 });
    }
}
//鼠标 移动
void Widget::mouseMoveEvent(QMouseEvent* e)
{
    mousex = e->x(); mousey = e->y();//获取鼠标坐标
}
//鼠标 松开
void Widget::mouseReleaseEvent(QMouseEvent* e)
{
    //恢复初始状态
    RightMouseDown = false;
    MidMouseDown = false;
    LeftMouseDown = false;
}
//键盘 事件
void Widget::keyPressEvent(QKeyEvent* key) {
    switch (key->key()) {
    case Qt::Key_W:
        //camDy = 1;
        Key_WDown = true;
        break;
    case Qt::Key_A:
        //camDx = -1;
        Key_ADown = true;
        break;
    case Qt::Key_S:
        //camDy = -1;
        Key_SDown = true;
        break;
    case Qt::Key_D:
        //camDx = 1;
        Key_DDown = true;
        break;
    }
}

void Widget::keyReleaseEvent(QKeyEvent* key) {
    //恢复初始状态
    switch (key->key()) {
    case Qt::Key_W:
        Key_WDown = false;
        break;
    case Qt::Key_A:
        Key_ADown = false;
        break;
    case Qt::Key_S:
        Key_SDown = false;
        break;
    case Qt::Key_D:
        Key_DDown = false;
        break;
    }
    if (Key_WDown+Key_ADown+Key_SDown+Key_DDown == 0){
        camDx = 0;
        camDy = 0;
    }
}


//射线单位矢量，在模型坐标系下的分量列阵
glm::vec4 Widget::get_ray(int mousex, int mousey, int screenWidth, int screenHeight, glm::mat4 matModel, glm::vec4& init_point) {
    glm::vec4 ndc((float)mousex * 2 / (float)screenWidth - 1, (float)mousey * 2 / (float)screenHeight - 1, 1.0f, 1.0f);//获取归一化坐标
    glm::vec4 pointView = glm::inverse(projection) * ndc;//获取相机坐标系的分量列阵
    pointView.y *= -1;//左手系转右手系
    pointView /= pointView.w;//把第四维度转成1
    glm::vec4 rayView = pointView - glm::vec4(0, 0, 0, 1);
    init_point = matModel * glm::inverse(view) * glm::vec4(0, 0, 0, 1);
    //show(rayView);;
    //返回归一化向量
    return glm::normalize(matModel * glm::inverse(view) * rayView);
}

glm::vec4 Widget::mousepick(int mousex, int mousey) {
    std::vector < glm::vec4 >  getpoints;
    PointCloudRenderer* pointcloud;
    hierarchy->root->callRecursively([&](HierarchyObject* obj) -> void {
        pointcloud = obj->getComponent<PointCloudRenderer>();
        if (pointcloud == nullptr)
            return;
        glm::vec4 init_point(0, 0, 0, 0);
        //show(init_point);
        //获取射线的单位向量
        glm::vec4 ray = get_ray(mousex, mousey, screenWidth, screenHeight, obj->worldToLocal(), init_point);
        //show(ray);
        //show(init_point);
        glm::vec4 point(0, 0, 0, 0);
        if (point.w) {
            getpoints.push_back(obj->localToWorld() * point);
        }
    });
    //在世界坐标系下比较和摄像机之间的距离
    if (getpoints.size() == 0)
        return glm::vec4(0, 0, 0, 0);
    else if (getpoints.size() == 1)
        return getpoints[0];
    else {
        int flag = 0;
        float d = sqrt(pow(getpoints[0].x - camPos.x, 2) + pow(getpoints[0].y - camPos.y, 2) + pow(getpoints[0].z - camPos.z, 2));
        for (int i = 1; i < getpoints.size(); i++) {
            float d1 = sqrt(pow(getpoints[i].x - camPos.x, 2) + pow(getpoints[i].y - camPos.y, 2) + pow(getpoints[i].z - camPos.z, 2));
            if (d > d1) {
                flag = i;
                d = d1;
            }
        }
        return getpoints[flag];
    }   
}

void Widget::fixedUpdate() {

    //如果鼠标按下，记录鼠标走的方位，进行相机旋转操作 
    if (RightMouseDown) {
        float dx = mousex - mouselastx;
        float dy = mousey - mouselasty;
        camRot = glm::rotate(camRot, -dy * 1.5f / sqrtf(screenWidth * screenHeight), glm::vec3(1, 0, 0));
        camRot = glm::rotate(camRot, -dx * 1.5f / sqrtf(screenWidth * screenHeight), glm::vec3(0, 1, 0));
        
    }
    if (LeftMouseDown) {
        glm::vec4 pickPoint = mousepick(mousex, mousey);
    }
    //滚轮旋转 结合鼠标位置进行摄像机平移和视角缩放，同时可以用wasd平移摄像机
  
    if (Key_WDown || Key_ADown || Key_SDown || Key_DDown || (wheeldelta)) {
        glm::vec3 z = glm::normalize(camPos - camTarget);
        glm::vec3 x = glm::normalize(glm::cross(camRot * glm::vec3(0.0f, 1.0f, 0.0f), z));//叉乘确定X轴
        glm::vec3 y = glm::normalize(camRot * glm::vec3(0.0f, 1.0f, 0.0f));
        if (wheeldelta) {//滚轮、鼠标控制
            camTarget += -0.03f * (mousex  - (int)screenWidth / 2) * ((float)exp(0.001 * wheeldelta)-1)* (float)(log10(1 + 0.3*distance)+0.1*distance) * x ;
            camTarget += 0.03f * (mousey - (int)screenHeight/2) * ((float)exp(0.001 * wheeldelta)-1) * (float)(log10(1 + 0.3*distance)+0.1*distance) * y;
            distance *= exp(0.001 * wheeldelta);
            wheeldelta = 0.0f;
        }
        else {//wasd控制
            //实现wasd同时控制，ad同时按可抵消，ws同时按可抵消
            camDx = -1 * Key_ADown + 1 * Key_DDown;
            camDy = -1 * Key_SDown + 1 * Key_WDown;
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
    res->enableAttributeArray(0);
    res->enableAttributeArray(1);
    return res;
}

void Widget::initializeGL()
{
    qDebug() << "-----------initialize GL--------------";
    this->initializeOpenGLFunctions();    //为当前上下文初始化提供OpenGL函数解析
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // 加载shader
    shaders["default"] = loadShader("foo");


    // 初始化相机位置姿态
    projection = glm::identity<glm::mat4>();
    view = glm::identity<glm::mat4>();
    camPos = { 0, 0, 3 };
    camRot = glm::identity<glm::quat>();
    camTarget = { 0.0, 0.0, 0.0 };
}


void Widget::resizeGL(int w, int h)
{
    glViewport(0.0f,0.0f,w,h);    //调整视口
    screenWidth = w;
    screenHeight = h;
}

void renderObjectRecursively(const glm::mat4& proj, const glm::mat4& view, const glm::mat4& parentTransform, HierarchyObject* obj) {
    assert(obj);
    glm::mat4 transform = parentTransform * obj->transform;
    for (int i = 0; i < obj->componentsCount(); i++) {
        Renderer* renderer = obj->getComponent<Renderer>(i);
        if (renderer) {
            renderer->onRender(proj, view, transform);
        }
    }
    for (int i = 0; i < obj->childrenCount(); i++) {
        renderObjectRecursively(proj, view, transform, obj->getChildren(i));
    }
}

void Widget::paintGL()
{
    assert(context());
    glClearColor(0.2f, 0.5f, 0.9f, 1.0f);    //清屏
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    //清除颜色缓冲

    //透视(视锥上下面之间的夹角，宽高比，即视窗的宽/高，近截面、远截面的深度)
    projection = glm::perspective(glm::radians(45.0f), screenWidth / (float)screenHeight, 0.01f, 300.0f);
    //相机位置更新
    //相机旋转矩阵，pos旋转后 = camRotation * pos旋转前
    //auto camRotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * glm::angleAxis(pitch, glm::vec3(1, 0, 0));
    camPos = camRot * glm::vec3(0, 0, distance) + camTarget;////可以实现绕不同点转 camTarget 表示相机拍摄视角的目标点
    //glm::vec3 camUp = camRot * glm::vec3(0, 1, 0);//相机y轴（相机正上方）的指向。。。默认相机永远在被观察物体的z轴上
    view = glm::transpose(glm::toMat4(camRot)) * glm::translate(glm::identity<glm::mat4>(), -camPos); //四元数转旋转矩阵

    assert(hierarchy);

    // update
    hierarchy->root->updateRecursively();
    // 渲染
    for (int i = 0; i < hierarchy->root->childrenCount(); i++) {
        renderObjectRecursively(projection, view, glm::identity<glm::mat4>(), hierarchy->root->getChildren(i));
    }

}
