#include "widget.h"
#include <stdio.h>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>


Widget::Widget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // 持续更新mousemove事件     
    setMouseTracking(true);


	setAcceptDrops(true);		//接收拖拽 


    setMinimumSize(100, 100);

    // 定时update    
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fixedUpdate()));
    timer->start(20);

    // 隐藏物体根节点     
    gizmosRoot = new HierarchyObject("gizmos");

    // 天空盒子     
    skybox = new SkyboxRenderer();

    // 坐标轴和网格       
    LineRenderer* xyzAxis = new LineRenderer();
    xyzAxis->setProp("continuous", false);
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

    // 平移旋转用handle 

    handleObj = new HierarchyObject("handles");
    LineRenderer* handle = new LineRenderer();
    handleObj->addComponent(handle);
    gizmosRoot->insertChild(0, handleObj);
    handle->setProp("lineWidth", 5.f);
    std::vector<Vertex> handleVertices = {
        // x axis   
        {{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},
        // y axis   
        {{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}},
        {{0.0, 1.0, 0.0}, {0.0, 1.0, 0.0}},
        // z axis   
        {{0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}},
        {{0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}},
    };
    handle->setVertices(handleVertices);

}

Widget::~Widget()
{
    delete gizmosRoot;
}

OpenGLFunctions * Widget::functions() const
{
    //qDebug << ((intptr_t)this);    
    assert(context());
    return context()->versionFunctions<OpenGLFunctions>();
}

//滚轮    
void Widget::wheelEvent(QWheelEvent* wheel) {
    Input::setMouseWheelDelta(wheel->delta());
}
//鼠标 按下     
void Widget::mousePressEvent(QMouseEvent* e) {
    Input::setMouseButton(e->button(), true);
}
//鼠标 移动    
void Widget::mouseMoveEvent(QMouseEvent* e) {
    mousex = e->x(); mousey = e->y();//获取鼠标坐标  
}
//鼠标 松开    
void Widget::mouseReleaseEvent(QMouseEvent* e) {
    Input::setMouseButton(e->button(), false);
}
//键盘 事件    
void Widget::keyPressEvent(QKeyEvent* key) {
    Input::setKey((Qt::Key) key->key(), true);
}
void Widget::keyReleaseEvent(QKeyEvent* key) {
    Input::setKey((Qt::Key) key->key(), false);
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

void Widget::getRayWorld(int mousex, int mousey, glm::vec3 out_source, glm::vec3 out_direction)
{
    glm::vec4 ndc((float)mousex * 2 / (float)screenWidth - 1, (float)mousey * 2 / (float)screenHeight - 1, 1.0f, 1.0f);//获取归一化坐标     
    glm::vec4 pointView = glm::inverse(projection) * ndc;//获取相机坐标系的分量列阵    
    pointView.y *= -1;//左手系转右手系    
    pointView /= pointView.w;//把第四维度转成1     
    glm::mat4 invView = glm::inverse(view);
    glm::vec4 rayView = pointView - glm::vec4(0, 0, 0, 1);
    out_source = invView * glm::vec4(0, 0, 0, 1);
    //返回归一化向量    
    glm::vec3 ray = invView * rayView;
    out_direction = glm::normalize(ray);
}


bool Widget::mousepick(int mousex, int mousey, HierarchyObject *& objout, int &iout) {
    std::vector < glm::vec4 >  getpoints;
    std::vector < HierarchyObject* > hitObjects;
    std::vector <int> pointIs;
    PointCloudRenderer* pointcloud;
    
    hierarchy->root->callRecursively([&](HierarchyObject* obj) -> void {
        pointcloud = obj->getComponent<PointCloudRenderer>();

        if (pointcloud == nullptr)
            return;
        glm::vec4 init_point(0, 0, 0, 0);
        //获取射线的单位向量    
        glm::vec3 ray = get_ray(mousex, mousey, screenWidth, screenHeight, obj->worldToLocal(), init_point);
        std::vector<Vertex> lvertices;

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
        float thre = 0.1 * pow((xmax[0] - xmin[0]) * (ymax[1] - ymin[1]) * (zmax[2] - zmin[2]) / (float)num, 1.0 / 3.0);
        //计算最远距离    
        float maxthre = 0;
        maxthre += (abs(init_point.x - xmin[0]) > abs(init_point.x - xmax[0])) * pow(init_point.x - xmin[0], 2);
        maxthre += (abs(init_point.x - xmin[0]) <= abs(init_point.x - xmax[0])) * pow(init_point.x - xmax[0], 2);
        maxthre += (abs(init_point.y - ymin[1]) > abs(init_point.y - ymax[1])) * pow(init_point.y - ymin[1], 2);
        maxthre += (abs(init_point.y - ymin[1]) <= abs(init_point.y - ymax[1])) * pow(init_point.y - ymax[1], 2);
        maxthre += (abs(init_point.z - zmin[2]) > abs(init_point.z - zmax[2])) * pow(init_point.z - zmin[2], 2);
        maxthre += (abs(init_point.z - zmin[2]) <= abs(init_point.z - zmax[2])) * pow(init_point.z - zmax[2], 2);
        maxthre = sqrt(maxthre);
        //在点云的三维空间范围内搜索，小于阈值即停止    

        while (true) {
            //算距离    
            float d = glm::l2Norm(glm::cross(glm::vec3(qpoint[0] - search.x, qpoint[1] - search.y, qpoint[2] - search.z), ray));
            if ( d < thre) {
                getpoints.push_back(obj->localToWorld() * glm::vec4(search.x,search.y,search.z,1));
                // 记录下命中点所在的obj和序号， 退出循环      
                hitObjects.push_back(obj);
                pointIs.push_back(pointI);  
                break;
            }
            
            float deltaDistance = glm::l2Norm(glm::vec3(qpoint[0] - search.x, qpoint[1] - search.y, qpoint[2] - search.z)) - thre;
            //改变搜索的起点    
            search += deltaDistance * ray;
            assert(glm::l2Norm(search) < 1e10);
            if (sqrt(pow(init_point.x - search.x, 2) + pow(init_point.y - search.y, 2) + pow(init_point.z - search.z, 2)) > maxthre)
                break;

            //更新搜索点    
            pointI = pointcloud->nearestSearch({ search.x,search.y, search.z });
            qpoint = pointcloud->getVertex(pointI).position();
        }
    });
    //在世界坐标系下比较和摄像机之间的距离    
    if (getpoints.size() == 0) {
        qDebug() << "请重新选择";
        objout = nullptr;
        return false;
    }
    else {
        int flag = 0; // 最近命中点所在的obj序号    
        float d = sqrt(pow(getpoints[0].x - camPos.x, 2) + pow(getpoints[0].y - camPos.y, 2) + pow(getpoints[0].z - camPos.z, 2));
        for (int i = 1; i < getpoints.size(); i++) {
            float d1 = sqrt(pow(getpoints[i].x - camPos.x, 2) + pow(getpoints[i].y - camPos.y, 2) + pow(getpoints[i].z - camPos.z, 2));
            if (d > d1) {
                flag = i;
                d = d1;
            }
        }

        // debug：标红点    
        //pointcloud = hitObjects[flag]->getComponent<PointCloudRenderer>();
        //std::vector<Vertex> vertices = pointcloud->getVertices();
        //vertices[pointIs[flag]] = Vertex(vertices[pointIs[flag]].position(), QVector3D(1.0, 0.0, 0.0));
        //pointcloud->setVertices(vertices);

        // 输出    
        objout = hitObjects[flag];
        iout = pointIs[flag];


        return true;
    }   
}

void Widget::fixedUpdate() {
    Input::beforeUpdate();
    bool RightMouseDown = Input::getMouseButton(Qt::MouseButton::RightButton);
    bool LeftMouseDown = Input::getMouseButton(Qt::MouseButton::LeftButton);
    bool Key_WDown = Input::getKey(Qt::Key::Key_W), Key_ADown = Input::getKey(Qt::Key::Key_A),
        Key_SDown = Input::getKey(Qt::Key::Key_S), Key_DDown = Input::getKey(Qt::Key::Key_D);
    float wheeldelta = Input::getMouseWheelDelta();

    //如果鼠标按下，记录鼠标走的方位，进行相机旋转操作    
    if (RightMouseDown) {
        float dx = mousex - mouselastx;
        float dy = mousey - mouselasty;
        camRot = glm::rotate(camRot, -dy * 1.5f / sqrtf(screenWidth * screenHeight), glm::vec3(1, 0, 0));
        camRot = glm::rotate(camRot, -dx * 1.5f / sqrtf(screenWidth * screenHeight), glm::vec3(0, 1, 0));
        
    }
    if (LeftMouseDown) {
        HierarchyObject * objout;
        int iout;
        bool found = mousepick(mousex, mousey,objout,iout);
        if (found) {
            qDebug() << " 左键选中  " << objout->name << " #" << iout;
            emit(onSelection(objout));
        }
        else {
            emit(onSelection(NULL));
        }
    }
    //滚轮旋转 结合鼠标位置进行摄像机平移和视角缩放，同时可以用wasd平移摄像机    
  
    if (Key_WDown || Key_ADown || Key_SDown || Key_DDown || (wheeldelta)) {
        glm::vec3 z = glm::normalize(camPos - camTarget);

        glm::vec3 x = glm::normalize(glm::cross(camRot * glm::vec3(0.0f, 1.0f, 0.0f), z));//叉乘确定X轴 

        glm::vec3 y = glm::normalize(camRot * glm::vec3(0.0f, 1.0f, 0.0f));
        if (wheeldelta) {//滚轮、鼠标控制     
            camTarget -= 0.002f * (float)wheeldelta * z;
            camTarget += 0.02f * (mousex  - (int)screenWidth / 2) * ((float)exp(0.001 * wheeldelta)-1)* (float)(log10(1 + 0.3*distance)+0.1*distance) * x ;
            camTarget -= 0.02f * (mousey - (int)screenHeight/2) * ((float)exp(0.001 * wheeldelta)-1) * (float)(log10(1 + 0.3*distance)+0.1*distance) * y;
            wheeldelta = 0.0f;
        }
        else {//wasd控制     
            //实现wasd同时控制，ad同时按可抵消，ws同时按可抵消     
            camDx = -1 * Key_ADown + 1 * Key_DDown;
            camDy = -1 * Key_SDown + 1 * Key_WDown;
            camTarget += (float)camDx * 0.05f * x * distance;
            camTarget += (float)camDy * 0.05f * y * distance;
            //qDebug() << camTarget.x << camTarget.y << camTarget.z;   
        }
    }

    // 更新lastx lasty     
    mouselastx = mousex;
    mouselasty = mousey;



    // -------默认相机矩阵初始化--------    
    //透视(视锥上下面之间的夹角，宽高比，即视窗的宽/高，近截面、远截面的深度)   
    projection = glm::perspective(glm::radians(45.0f), screenWidth / (float)screenHeight, 0.01f, 300.0f);
    //相机位置更新   
    //相机旋转矩阵，pos旋转后 = camRotation * pos旋转前     
    //auto camRotation = glm::angleAxis(yaw, glm::vec3(0, 1, 0)) * glm::angleAxis(pitch, glm::vec3(1, 0, 0));   
    camPos = camRot * glm::vec3(0, 0, distance) + camTarget;////可以实现绕不同点转 camTarget 表示相机拍摄视角的目标点     
    //glm::vec3 camUp = camRot * glm::vec3(0, 1, 0);//相机y轴（相机正上方）的指向。。。默认相机永远在被观察物体的z轴上     
    view = glm::transpose(glm::toMat4(camRot)) * glm::translate(glm::identity<glm::mat4>(), -camPos); //四元数转旋转矩阵     


    // --------update------
    hierarchy->root->updateRecursively();

    // --------trail-------
    if (Input::getKeyDown(Qt::Key::Key_T)) {
        
    }
    /*
    if (Input::getKeyDown(Qt::Key::Key_H)) {
        auto r = hierarchy->root->getChildren("trailTest")->getComponent<LineRenderer>();
        r->setProp("enabled", !r->getProp("enabled").toBool());
    }
    */
    if (currentTrail != NULL) {
        currentTrailTime += videoRecordSpeed;
        if (currentTrailTime > currentTrail->keypoints.size() - 1) {
            currentTrail = NULL;
            currentTrailTime = 0;
            emit(videoRecordFinish(videoRecordFlag));
            videoRecordFlag = false;
            
        }
        else {
            glm::mat4 camMat = currentTrail->interpolate(currentTrailTime);

            view = glm::inverse(camMat); 
            //qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz")<<"begin";
            if (videoRecordFlag) {
                QPixmap p = this->grab(QRect(0, 0, screenWidth, screenHeight));
                //QString filePathName = "screen/";
                //filePathName += QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz");
                //filePathName += ".png";
                video.push_back(p);
                //qDebug()<< QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss-zzz")<<"end";
            }
        }
    }

    // ----- handle -----    
    if (hierarchy->lastSelected == NULL) {
        handleObj->enabled = false;
    }
    else {
        handleObj->enabled = true;
        glm::mat4 rawMat = hierarchy->lastSelected->localToWorld();
        // 按屏幕内固定大小缩放   
        glm::vec3 xw = rawMat * glm::vec4(1, 0, 0, 0);
        glm::vec3 yw = rawMat * glm::vec4(0, 1, 0, 0);
        glm::vec3 zw = rawMat * glm::vec4(0, 0, 1, 0);
        float averageAxisLength = (glm::length(xw) + glm::length(yw) + glm::length(zw)) / 3.0f;
        glm::vec3 pos = rawMat * glm::vec4(0, 0, 0, 1);
        glm::vec3 camPos = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
        float d = glm::length(pos - camPos);
        handleObj->transform = glm::scale(rawMat, 0.1f / averageAxisLength * d * glm::vec3(1, 1, 1));
    }


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
    shaders["skybox"] = loadShader("skybox");


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
    if (!obj->enabled) {
        return;
    }
    glm::mat4 transform = parentTransform * obj->transform;
    for (int i = 0; i < obj->componentsCount(); i++) {
        Renderer* renderer = obj->getComponent<Renderer>(i);
        if (renderer && renderer->getProp("enabled").toBool()) {
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

    assert(hierarchy);


    // --------渲染--------   
    // 渲染天空     
    skybox->shader = shaders["skybox"];
    skybox->onRender(functions(), projection, view, view);// 第三个参数没用     

    // 渲染普通物体      
    for (int i = 0; i < hierarchy->root->childrenCount(); i++) {
        renderObjectRecursively(projection, view, glm::identity<glm::mat4>(), hierarchy->root->getChildren(i));
    }

    // 渲染gizmos      
    renderObjectRecursively(projection, view, glm::identity<glm::mat4>(), gizmosRoot);

}


void Widget::dropEvent(QDropEvent * e) {		//拖拽松手后操作 

	const QMimeData* qm = e->mimeData();
	std::string path = qm->urls()[0].toLocalFile().toStdString();
	QString abs_path = QCoreApplication::applicationDirPath();

	std::string re_path = QDir(abs_path).relativeFilePath(QString::fromStdString(path)).toStdString();

	//将绝对路径转为相对路径 
	std::fstream test_file;
	re_path.erase(re_path.begin());
	//这里打开了拖入的文件 

	qDebug() << re_path.c_str() << endl;

	emit drag_signal(re_path);
}


void Widget::dragEnterEvent(QDragEnterEvent * e) {				//接收所有的拖拽事件(后续可以改为根据文件的扩展名进行筛选) 

	e->acceptProposedAction();
}



void Widget::onRecordVideo1Wigdet(float speed, bool RecordOrPreview){
    currentTrail = hierarchy->root->getChildren("trailTest")->getComponent<Trail>();
    currentTrailTime = 0;
    videoRecordSpeed = 0.0002f * speed;
    videoRecordFlag = RecordOrPreview;
    video.clear();
}
void Widget::onSaveVideo1Widget() {
    
    
    cv::VideoWriter writer;
    int videoFps = 30;
    emit(videoSaveFinish());
    writer.open("E:\\github\\gldemo\\data\\saveVideo\\trail3D.avi", 
        cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 
        30.0, 
        cv::Size((int)screenWidth,(int)screenHeight), 
        true
    );
    if (!writer.isOpened()) {
        qDebug() << "打开视频文件失败，请确认是否为合法输入    ";
        return;
    }
    QtToOpencv::ImageConversion *a = new QtToOpencv::ImageConversion();
    //qDebug() << "videoFPS:" << videoFps << (int)screenWidth<<(int)screenHeight;
    for (int i = 0; i < video.size(); i++) {
        cv::Mat frame;
        frame = a->QPixmapToCvMat(video[i]);
        //cv::imshow("111", frame);
        //cv::waitKey(10);

        cv::cvtColor(frame, frame, cv::COLOR_RGBA2RGB);

        //qDebug() <<  frame.cols << frame.rows;
        writer.write(frame);
    }
    writer.release();
    delete a;
}