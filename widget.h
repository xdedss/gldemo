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
    // ��¼��Ļ����
    unsigned int screenWidth, screenHeight;
    // ��¼���״̬
    bool RightMouseDown = false;
    bool MidMouseDown = false;
    bool LeftMouseDown = false;
    bool WheelRotate = false;
    //��¼����״̬
    bool Key_ADown = false;
    bool Key_WDown = false;
    bool Key_SDown = false;
    bool Key_DDown = false;
    // �����̬���
    glm::vec3 camPos, camTarget, camUp;
    glm::mat4 view, projection;
    float pitch = 0, yaw = 0;
    //wasd ���������
    int camDx = 0, camDy = 0;
    float distance = 5;
    int mouselastx = 0, mouselasty = 0, mousex = 0, mousey = 0;
    float wheeldelta = 0;
    int windowX = 0,windowY = 0;

    // shader
    QOpenGLShaderProgram* m_program;
    // ģ��
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
