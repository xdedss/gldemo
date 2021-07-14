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


// 主窗口定义
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionopen_triggered();
    void onTreeViewCustomContextMenu(const QPoint &);

    void onTreeViewAddObject();
    void onTreeViewRemoveObject();

    void onWidgetSelection(HierarchyObject* obj);
    void btn_slot1();
    void btn_slot2();
    void btn_slot3();

private:
    Ui::MainWindow *ui;

    QMenu* treeContextMenu;//节点上右键菜单  
    QMenu* treeContextMenuSpace;//空白处右键菜单  

    //QStandardItemModel* hierarchy;
    //QStandardItem* modelsParent;
    //QStandardItem* trailsParent;

    HierarchyModel* hierarchy; // 场景树  

    PointCloudRenderer* MainWindow::importPointCloud(const QString& path, float initialScale); // 直接向场景中放入一个点云模型  

};

#endif // MAINWINDOW_H
