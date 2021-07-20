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
    * rotation顺序为y->x->z。单位为 角度制
    * 规定 x转角范围为[-pi/2,pi/2],y/z转角范围为[-pi,pi]
    */
    void showObjectInfo(HierarchyObject* obj);
    /**
    * @brief 在Qt界面根据输入的信息，更新物体的平移，旋转，缩放
    * @attention 
    * 单位为 角度制
    * 输入时，x转角范围可以为[-pi,pi]，而后再次选中该物体时，xyz转角自动转化为showObjectInfo函数规定的范围
    */
    void updateObjectInfo(HierarchyObject* obj);

signals:
    /** @brief 录像开启信号，从mainwindow传到widget;携带参数为摄像机移动速度、录像是否存储，不存储则为预览模式 */
    void onRecordVideo2Widget(float speed, bool RecordOrPreview);
    /** @brief 录像关闭信号，从mainwindow传到recordwindow；携带参数为录像是否存储，存储则开启save功能 */
    void offRecordVideo2recordWindow(bool RecordOrPreview);
    /** @brief 视频开始保存信号，从mainwindow传到widget */
    void onSaveVideo2Widget();
    /** @brief 视频保存结束信号，从mainwindow传到widget */
    void offSaveVideo2recordWindow();

private slots:
    void on_actionopen_triggered();
    void onTreeViewCustomContextMenu(const QPoint &);

    void onTreeViewAddObject();
    void onTreeViewRemoveObject();

    void onWidgetSelection(HierarchyObject* obj);
    void onWidgetTransformEdited(HierarchyObject* obj);
    void onEdited();
    void ObjectSelected(const QItemSelection& selected, const QItemSelection& deselected);



	void drag_solt(std::string re_path);		
    void on_actionvideoRecord_triggered();

    
    //接收录像开始的信号，并发送给widget
    void onRecordVideo1MainWindow(float speed, bool RecordOrPreview);
    //接收录像结束的信号       
    void offRecordVideo1MainWindow(bool RecordOrPreview);

    //接收保存视频的信号，并发送给widget    
    void onSaveVideo1MainWindow();
    //接收保存结束的信号    
    void offSaveVideo1MainWindow();


    void on_actionTranslation_toggled(bool arg1);

    void on_actionRotate_toggled(bool arg1);

    void on_actionScale_toggled(bool arg1);

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
    recordWindow* record;
};

#endif // MAINWINDOW_H
