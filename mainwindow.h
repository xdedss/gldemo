#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstandarditemmodel.h>
#include <set>
#include <qitemselectionmodel.h>
#include "pointcloud.h"
#include "nanoflann.hpp"
#include "HierarchyModel.h"
#include "HierarchyObject.h"
#include "PointCloudRenderer.h"


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

private:
    Ui::MainWindow *ui;

    //QStandardItemModel* hierarchy;
    //QStandardItem* modelsParent;
    //QStandardItem* trailsParent;

    HierarchyModel* hierarchy;

    PointCloudRenderer* MainWindow::importPointCloud(const QString& path, float initialScale);

};

#endif // MAINWINDOW_H
