#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <qtimer.h>

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
    unsigned int screenWidth, screenHeight;
    glm::vec3 camPos, camTarget, camUp;
    glm::mat4 view, model, projection;
    float pitch = 0, yaw = 0;
    float modelAngle = 0;
    int camDx = 0, camDy = 0;//wasd ÉãÏñ»úÂþÓÎ
    bool RightMouseDown = false;
    bool MidMouseDown = false;
    bool WheelRotate = false;
    float distance = 5;
    int mouselastx = 0, mouselasty = 0, mousex = 0, mousey = 0;
    QOpenGLShaderProgram* m_program;
    //QOpenGLVertexArrayObject m_vao;
    //QOpenGLBuffer m_vertexBuffer;
    PointCloud* pointCloud;

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
