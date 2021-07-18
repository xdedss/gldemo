#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <qtimer.h>

#include <vector>
#include <map>

// OpenGL
//#include <QOpenGLExtraFunctions>
#include <qopenglshaderprogram.h>
#include <qopenglvertexarrayobject.h>
#include <qopenglbuffer.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <QDebug>
#include <QKeyEvent>

#include "vertex.h"
//#include "pointcloud.h"
#include "futils.h"
#include "HierarchyModel.h"
#include "HierarchyObject.h"
#include "Renderer.h"
#include "PointCloudRenderer.h"
#include "LineRenderer.h"
#include "SkyboxRenderer.h"
#include "commondefs.h"
#include "Input.h"
#include "Trail.h"

#include <QDragEnterEvent>
#include <string>
#include <QMimeData>
#include <QDropEvent>
#include <QUrl>
#include <qcoreapplication.h>
#include <qdir.h>			//拖拽文件所需的库函数

//保存图片的引用    
#include <Qt3Drender/qcamera.h>
#include <Qt3Drender/qrendercapture.h>
#include <QPixmap>
#include <QImage>
#include <QDateTime>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMovie>


//回放视频   
#include "mainwindow.h"
#include <QLabel>


class Widget : public QOpenGLWidget, protected OpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    OpenGLFunctions* functions() const;

    // shader
    std::map<QString, QOpenGLShaderProgram*> shaders;
    // 模型  
    //std::vector<PointCloud*> pointClouds;
    HierarchyModel* hierarchy;

    HierarchyObject* gizmosRoot;

    SkyboxRenderer* skybox;
    
    Trail* currentTrail = NULL;
    float currentTrailTime = 0;

    HierarchyObject* handleObj;

    std::vector<QPixmap> video;

    void setHierarchy(HierarchyModel* hierarchy) {
        hierarchy->widget = this;
        this->hierarchy = hierarchy;
    }

private:
    // 记录屏幕长宽  
    unsigned int screenWidth, screenHeight;
    // 相机姿态相关  
    glm::vec3 camPos, camTarget;
    glm::quat camRot;
    glm::mat4 view, projection;
    //wasd 摄像机漫游  
    int camDx = 0, camDy = 0;
    float distance = 5;
    int mouselastx = 0, mouselasty = 0, mousex = 0, mousey = 0;
    //float wheeldelta = 0;
    int windowX = 0,windowY = 0;
    float videoRecordSpeed = 0.01;

private slots:
    void fixedUpdate();
    void onRecordVideo1Wigdet(float speed);


signals:
    void onSelection(HierarchyObject* obj);
	void drag_signal(std::string re_path);		//实现拖拽的信号函数

private:
    bool mousepick(int mousex, int mousey, HierarchyObject*& objout, int& iout);
    glm::vec3 get_ray(int mousex, int mousey, int screenWidth, int screenHeight,
        glm::mat4 matModel, glm::vec4& init_point);
    void getRayWorld(int mousex, int mousey, glm::vec3 out_source, glm::vec3 out_direction);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void handleDefaultShader(Renderer * renderer);
    void renderObjectRecursively(const glm::mat4 & proj, const glm::mat4 & view, const glm::mat4 & parentTransform, HierarchyObject * obj);
    void paintGL() override;


    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);

    void wheelEvent(QWheelEvent* wheel);

    void keyPressEvent(QKeyEvent* key);
    void keyReleaseEvent(QKeyEvent* key);

	void dropEvent(QDropEvent * e);
	void dragEnterEvent(QDragEnterEvent * e);	//实现拖拽的函数
};
#endif // WIDGET_H
