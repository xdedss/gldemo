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

    gizmosRoot = new HierarchyObject("gizmos");
    LineRenderer* xyzAxis = new LineRenderer();
    xyzAxis->continuous = false;
    std::vector<Vertex> xyAxisVertices = {
        // x axis
        {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        {{5.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        // y axis
        {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}},
        {{0.0, 5.0, 0.0}, {0.0, 1.0, 0.0}},
        // z axis
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
        {{0.0, 0.0, 5.0}, {0.0, 0.0, 1.0}},
    };
    float meshGap = 1;
    int meshCount = 10;
    QVector3D meshColor = { 0.4, 0.4, 0.4 };
    for (int i = -meshCount; i <= meshCount; i++) {
        xyAxisVertices.push_back({ {meshGap * i, 0, -meshGap * meshCount}, meshColor });
        xyAxisVertices.push_back({ {meshGap * i, 0, meshGap * meshCount}, meshColor });
        xyAxisVertices.push_back({ {-meshGap * meshCount, 0, meshGap * i}, meshColor });
        xyAxisVertices.push_back({ {meshGap * meshCount, 0, meshGap * i}, meshColor });
    }
    xyzAxis->setVertices(xyAxisVertices);
    gizmosRoot->addComponent(xyzAxis);
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
        auto cloud = hierarchy->root->getChildren("bun180.ply")->getComponent<PointCloudRenderer>();
        int idx = cloud->nearestSearch({ 0, 0, 0 });
        qDebug() << "search " << idx << "  " << cloud->getVertex(idx).position();
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
glm::vec3 Widget::get_ray(int mousex, int mousey, int screenWidth, int screenHeight, glm::mat4 matModel, glm::vec4& init_point) {
    glm::vec4 ndc((float)mousex * 2 / (float)screenWidth - 1, (float)mousey * 2 / (float)screenHeight - 1, 1.0f, 1.0f);//获取归一化坐标
    glm::vec4 pointView = glm::inverse(projection) * ndc;//获取相机坐标系的分量列阵
    pointView.y *= -1;//左手系转右手系
    pointView /= pointView.w;//把第四维度转成1
    glm::vec4 rayView = pointView - glm::vec4(0, 0, 0, 1);
    init_point = matModel * glm::inverse(view) * glm::vec4(0, 0, 0, 1);
    //show(rayView);;
    //返回归一化向量
    glm::vec4 ray = glm::normalize(matModel * glm::inverse(view) * rayView);
    return glm::vec3(ray.x, ray.y, ray.z);
}




glm::vec4 Widget::mousepick(int mousex, int mousey) {
    std::vector < glm::vec4 >  getpoints;
    PointCloudRenderer* pointcloud;
    int n = 1;
    hierarchy->root->callRecursively([&](HierarchyObject* obj) -> void {
        pointcloud = obj->getComponent<PointCloudRenderer>();
        LineRenderer* l = obj->getComponent<LineRenderer>();
        if (pointcloud == nullptr)
            return;
        qDebug() << obj->name;
        //if (obj->name != "bun180.ply")
            //return;

        qDebug() << "NO " << n;
        n++;
        glm::vec4 init_point(0, 0, 0, 0);
        //获取射线的单位向量
        glm::vec3 ray = get_ray(mousex, mousey, screenWidth, screenHeight, obj->worldToLocal(), init_point);
        std::vector<Vertex> lvertices;
        lvertices.push_back({ {init_point.x,init_point.y,init_point.z},{0,1.0,1.0} });
        lvertices.push_back({ {init_point.x + 300 * ray.x,init_point.y + 300 * ray.y,init_point.z + 300 * ray.z }, { 0,1.0,1.0 } });
        

        l->continuous = false;


        //求射线上最近的点
        int pointI = pointcloud->nearestSearch({init_point.x,init_point.y, init_point.z});
        auto qpoint = pointcloud->getVertex(pointI).position();


        //每次搜索的起点
        glm::vec3 search(init_point.x, init_point.y, init_point.z);

        //获取包含模型的立方体
        QVector3D xmin, xmax, ymin, ymax, zmin, zmax;
        xmin = pointcloud->getVertex(pointcloud->nearestSearch({ -10000,0,0 })).position();
        xmax = pointcloud->getVertex(pointcloud->nearestSearch({  10000,0,0 })).position();
        ymin = pointcloud->getVertex(pointcloud->nearestSearch({ 0,-10000,0 })).position();
        ymax = pointcloud->getVertex(pointcloud->nearestSearch({ 0, 10000,0 })).position();
        zmin = pointcloud->getVertex(pointcloud->nearestSearch({ 0,0,-10000 })).position();
        zmax = pointcloud->getVertex(pointcloud->nearestSearch({ 0,0, 10000 })).position();
        int num = pointcloud->vertexCount();
        float thre = 0.1*pow((xmax[0]-xmin[0])*(ymax[1]-ymin[1])*(zmax[2]-zmin[2])/(float)num,1.0/3.0);
        qDebug() <<"thre" <<thre;
        //在点云的三维空间范围内搜索，小于阈值即停止



        while (true) {
            //算距离
            qDebug() << "search:" << search.x << search.y << search.z;
            
            lvertices.push_back({ {search.x , search.y , search.z} ,{0,0,1.0} });
            lvertices.push_back({ {qpoint[0] , qpoint[1] , qpoint[2]} ,{0,0,1.0} });
            float d = glm::l2Norm(glm::cross(glm::vec3(qpoint[0] - search.x, qpoint[1] - search.y, qpoint[2] - search.z), ray));
            qDebug() << "d:" << d;
            if ( d < thre) {
                qDebug() << "d:" << d;
                getpoints.push_back(obj->localToWorld() * glm::vec4(search.x,search.y,search.z,1));
                std::vector<Vertex> vertices = pointcloud->getVertices();
                vertices[pointI] = Vertex(vertices[pointI].position(), QVector3D(1.0, 0.0, 0.0));
                pointcloud->setVertices(vertices);
                break;
            }
            
            float deltaDistance = glm::l2Norm(glm::vec3(qpoint[0] - search.x, qpoint[1] - search.y, qpoint[2] - search.z)) - thre;
            //改变搜索的起点
            search += deltaDistance * ray;

            lvertices.push_back({ {search.x , search.y , search.z} ,{0,0,1.0} });
            lvertices.push_back({ {qpoint[0] , qpoint[1] , qpoint[2]} ,{0,0,1.0} });
            
            if (search.x < xmin[0] * 1.5 || search.x > xmax[0] * 1.5
                || search.y < ymin[1] * 1.5 || search.y > ymax[1] * 1.5
                || search.z < zmin[2] * 1.5 || search.z > zmax[2] * 1.5)
                break;

            //更新搜索点
            pointI = pointcloud->nearestSearch({ search.x,search.y, search.z });
            qpoint = pointcloud->getVertex(pointI).position();
        }

        l->setVertices(lvertices);
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
            camTarget += 0.002f * (float)wheeldelta * z;
            camTarget += -0.02f * (mousex  - (int)screenWidth / 2) * ((float)exp(0.001 * wheeldelta)-1)* (float)(log10(1 + 0.3*distance)+0.1*distance) * x ;
            camTarget += 0.02f * (mousey - (int)screenHeight/2) * ((float)exp(0.001 * wheeldelta)-1) * (float)(log10(1 + 0.3*distance)+0.1*distance) * y;
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
    res->addShaderFromSourceFile(QOpenGLShader::Vertex, ("./shaders/" + name + ".vert").c_str());
    res->addShaderFromSourceFile(QOpenGLShader::Fragment, ("./shaders/" + name + ".frag").c_str());
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
    shaders["default"] = loadShader("default");


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

void Widget::handleDefaultShader(Renderer* renderer) {
    PointCloudRenderer* cloud = dynamic_cast<PointCloudRenderer*>(renderer);
    if (cloud) {
        if (cloud->shader == NULL) {
            cloud->shader = shaders["default"];
        }
    }
    LineRenderer * line = dynamic_cast<LineRenderer*>(renderer);
    if (line) {
        if (line->shader == NULL) {
            line->shader = shaders["default"];
        }
    }
}

void Widget::renderObjectRecursively(const glm::mat4& proj, const glm::mat4& view, const glm::mat4& parentTransform, HierarchyObject* obj) {
    assert(obj);
    glm::mat4 transform = parentTransform * obj->transform;
    for (int i = 0; i < obj->componentsCount(); i++) {
        Renderer* renderer = obj->getComponent<Renderer>(i);
        if (renderer) {
            handleDefaultShader(renderer);
            renderer->onRender(functions(), proj, view, transform);
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

    renderObjectRecursively(projection, view, glm::identity<glm::mat4>(), gizmosRoot);

}
