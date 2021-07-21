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

//保存视频     
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "qttoopencv.h"
#include <opencv2/videoio.hpp>


/**
* @class Widget 
* @brief OpenGL渲染窗口
*/
class Widget : public QOpenGLWidget, protected OpenGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    /** @brief 获取含有OpenGL各种函数的对象 */
    OpenGLFunctions* functions() const;

    /** @brief 储存加载好的shader */
    std::map<QString, QOpenGLShaderProgram*> shaders;

    /** @brief 要渲染的场景 */
    HierarchyModel* hierarchy;

    /** @brief 显示辅助信息用的根节点 */  
    HierarchyObject* gizmosRoot;

    /** @brief 天空渲染（单独处理） */
    SkyboxRenderer* skybox;

    /** @brief 局部坐标轴节点（单独处理） */
    HierarchyObject* handleObj;
    
    /** @brief 当前播放的路径 */
    Trail* currentTrail = NULL;
    /** @brief 当前播放的路径进度 */
    float currentTrailTime = 0;
    
    /** @brief 鼠标左键编辑模式 
     *  @note  0=无 1=平移 2=旋转 3=缩放 
     */
    int LMBMode = 0;

    /** @brief 储存视频录制结果 */
    std::vector<QPixmap> video;

    /** @brief 将场景管理器绑定到此渲染窗口 */
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
    int windowX = 0,windowY = 0;
    float videoRecordSpeed = 0.01;
    bool videoRecordFlag = false;


private slots:
    /** @brief 接收时钟信号，更新帧 */
    void fixedUpdate();
    /** @brief 接收录制视频信号 */
    void onRecordVideo1Wigdet(float speed, bool RecordOrPreview);
    /** @brief 接收保存视频信号 */
    void onSaveVideo1Widget(QString savePath);

signals:
    /** @brief 发送用户鼠标选取模型的信号 */
    void onSelection(HierarchyObject* obj);
    /** @brief 发送用户鼠标编辑物体信号 */
    void onTransformEdited(HierarchyObject* obj);
    /** @brief 实现拖拽的信号函数 */
	void drag_signal(std::string re_path);
    /** @brief 视频录制结束信号 */
    void videoRecordFinish(bool RecordOrPreview);
    /** @brief 视频保存结束信号 */
    void videoSaveFinish();

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
