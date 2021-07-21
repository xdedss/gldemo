#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstandarditemmodel.h>
#include <set>
#include <qitemselectionmodel.h>
#include <qabstractitemmodel.h>
#include "pointcloud.h"
#include "nanoflann.hpp"
#include "HierarchyModel.h"
#include "HierarchyObject.h"
#include "PointCloudRenderer.h"
#include "LineRenderer.h"
#include <sstream>
#include <glm/gtx/matrix_decompose.hpp>


//新窗口 
#include "recordwindow.h"

//保存视频获取时，获取路径   
#include "QString"
#include "QDir"
#include "QFileDialog"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    

private:
    /** 
    * @brief 在Qt界面显示物体的平移，旋转，缩放变换信息 
    * @attention
    * rotation顺序为y->x->z。单位为 角度制;   \n
    * 规定 x转角范围为[-90,90],y/z转角范围为[-180,180]
    */
    void showObjectInfo(HierarchyObject* obj);
    /**
    * @brief 在Qt界面根据输入的信息，更新物体的平移，旋转，缩放
    * @attention 
    * 单位为 角度制   \n
    * 输入时，x转角范围可以为[-180,180]，而后再次选中该物体时，xyz转角自动转化为showObjectInfo函数规定的范围
    */
    void updateObjectInfo(HierarchyObject* obj);

signals:
    /** 
    * @brief 录像开启信号，从mainwindow传到widget   \n
    * 携带参数为摄像机移动速度、录像是否存储，不存储则为预览模式 
    */
    void onRecordVideo2Widget(float speed, bool RecordOrPreview);
    /** 
    * @brief 录像关闭信号，从mainwindow传到recordwindow    \n
    * 携带参数为录像是否存储，存储则开启save功能 
    */
    void offRecordVideo2recordWindow(bool RecordOrPreview);
    /** @brief 视频开始保存信号，从mainwindow传到widget */
    void onSaveVideo2Widget(QString savePath);
    /** @brief 视频保存结束信号，从mainwindow传到widget */
    void offSaveVideo2recordWindow();

private slots:
    void on_actionopen_triggered();
    void onTreeViewCustomContextMenu(const QPoint &);
    
    /** @brief 接收场景树添加新物体的信号，添加新物体 */
    void onTreeViewAddObject();
    /** @brief 接收场景树删除物体的信号，删除物体 */
    void onTreeViewRemoveObject();
    /** @brief 接收<画面内>鼠标点击物体选择的信号，显示高亮*/
    void onWidgetSelection(HierarchyObject* obj);
    /** @brief 接收<画面内>鼠标选择、挪动物体的信号，显示物体信息 */
    void onWidgetTransformEdited(HierarchyObject* obj);

    /** @brief 接收<Qt界面>右上角文本框被编辑的信号，实时改变Object的transform矩阵 */
    void onEdited();
    /** @brief 接收<Qt界面>左侧场景树栏中Object选择的信号，使选中物体的状态显示且可编辑，若无选中物体，则lock文本框 */
    void ObjectSelected(const QItemSelection& selected, const QItemSelection& deselected);

    /** @brief 接收加载点云信号,拖拽点云文件进行加载  */
	void drag_solt(std::string re_path);
    
    /** @brief 接收recordwindow点击信号，显示recordWindow  */
    void on_actionvideoRecord_triggered();
    /** @brief 接收录像开始的信号，并发送给widget */
    void onRecordVideo1MainWindow(float speed, bool RecordOrPreview);
    /** @brief 接收录像结束的信号 */
    void offRecordVideo1MainWindow(bool RecordOrPreview);
    /** @brief 接收保存视频的信号，并发送给widget    */
    void onSaveVideo1MainWindow();
    /** @brief 接收保存结束的信号    */
    void offSaveVideo1MainWindow();

    /** @brief 接收鼠标左键点击平移按钮的信号，开启 拖拽平移物体 功能 */
    void on_actionTranslation_toggled(bool arg1);
    /** @brief 接收鼠标左键点击旋转按钮的信号，开启 拖拽旋转物体 功能 */
    void on_actionRotate_toggled(bool arg1);
    /** @brief 接收鼠标左键点击缩放按钮的信号，开启 拖拽缩放物体 功能 */
    void on_actionScale_toggled(bool arg1);
    /** @brief 接收鼠标左键点击物体选择的信号，开启 选择物体 功能,程序初始化即为此功能 */
    void on_actionCursor_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    QMenu* treeContextMenu;
    QMenu* treeContextMenuSpace;

    //QStandardItemModel* hierarchy;
    //QStandardItem* modelsParent;
    //QStandardItem* trailsParent;

    HierarchyModel* hierarchy;

    PointCloudRenderer* MainWindow::importPointCloud(const QString& path, float initialScale); 
    RecordWindow* record;
};

#endif // MAINWINDOW_H
