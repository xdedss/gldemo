#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qstandarditemmodel.h>
#include <set>
#include "pointcloud.h"

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
    QStandardItemModel* hierarchy;
    QStandardItem* modelsParent;
    QStandardItem* trailsParent;
    std::set<PointCloud*> pointClouds;

    PointCloud* importPointCloud(const QString& path, float initialScale);

};

#endif // MAINWINDOW_H
