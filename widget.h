#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <qtimer.h>

#include <vector>

// OpenGL
//#include <QOpenGLExtraFunctions>
#include <qopenglfunctions_4_3_core.h>
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
#include "pointcloud.h"
#include "futils.h"

class Widget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();



private:
    // 记录屏幕长宽
    unsigned int screenWidth, screenHeight;
    // 记录鼠标状态
    bool RightMouseDown = false;
    bool MidMouseDown = false;
    bool LeftMouseDown = false;
    bool WheelRotate = false;
    // 相机姿态相关
    glm::vec3 camPos, camTarget;
    glm::quat camRot;
    glm::mat4 view, projection;
    //wasd 摄像机漫游
    int camDx = 0, camDy = 0;
    float distance = 5;
    int mouselastx = 0, mouselasty = 0, mousex = 0, mousey = 0;
    float wheeldelta = 0;
    int windowX = 0,windowY = 0;

    // shader
    QOpenGLShaderProgram* m_program;
    // 模型
    std::vector<PointCloud*> pointClouds;


private slots:
    void fixedUpdate();


protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;


    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);

    void wheelEvent(QWheelEvent* wheel);

    void keyPressEvent(QKeyEvent* key);
    void keyReleaseEvent(QKeyEvent* key);
};
#endif // WIDGET_H
