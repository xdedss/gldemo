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



private slots:
    void fixedUpdate();

signals:
    void onSelection(HierarchyObject* obj);

private:
    bool mousepick(int mousex, int mousey, HierarchyObject*& objout, int& iout);
    glm::vec3 get_ray(int mousex, int mousey, int screenWidth, int screenHeight,
        glm::mat4 matModel, glm::vec4& init_point);

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
};
#endif // WIDGET_H
