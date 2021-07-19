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
    

signals:
    void onRecordVideo2Widget(float speed);
    void offRecordVideo2recordWindow();
    void onSaveVideo2Widget();
    void offSaveVideo2recordWindow();

private slots:
    void on_actionopen_triggered();
    void onTreeViewCustomContextMenu(const QPoint &);

    void onTreeViewAddObject();
    void onTreeViewRemoveObject();

    void onWidgetSelection(HierarchyObject* obj);
    void onEdited();
    void ObjectSelected(const QItemSelection& selected, const QItemSelection& deselected);



	void drag_solt(std::string re_path);		
    void on_actionvideoRecord_triggered();

    
    //接收录像开始的信号，并发送给widget
    void onRecordVideo1MainWindow(float speed);
    //接收录像结束的信号    
    void offRecordVideo1MainWindow();

    //接收保存视频的信号，并发送给widget
    void onSaveVideo1MainWindow();
    //接收保存结束的信号
    void offSaveVideo1MainWindow();


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
