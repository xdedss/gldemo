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


// �����ڶ���
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
    void onEdited();
    void scaleChange(float scalex, float scaley, float scalez, HierarchyObject *&obj);
    void ObjectSelected(const QItemSelection& selected, const QItemSelection& deselected);



private:
    Ui::MainWindow *ui;
    QMenu* treeContextMenu;//�ڵ����Ҽ��˵�  
    QMenu* treeContextMenuSpace;//�հ״��Ҽ��˵�  

    //QStandardItemModel* hierarchy;
    //QStandardItem* modelsParent;
    //QStandardItem* trailsParent;

    HierarchyModel* hierarchy; // ������  

    PointCloudRenderer* MainWindow::importPointCloud(const QString& path, float initialScale); // ֱ���򳡾��з���һ������ģ��  

};

#endif // MAINWINDOW_H
