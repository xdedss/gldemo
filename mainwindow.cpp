#include "mainwindow.h"
#include "ui_mainwindow.h"

// 主窗口初始化
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Done");

    // 分割线默认比例
    ui->splitter_hor->setStretchFactor(0, 1);
    ui->splitter_hor->setStretchFactor(1, 3);

    // 左侧树状结构 以后可以做成和场景内容相关联
    QStringList thead;
    thead << "Hierarchy";
    hierarchy = new QStandardItemModel();
    hierarchy->setHorizontalHeaderLabels(thead);
    QStandardItem* parent1 = new QStandardItem(QStringLiteral("foo"));
    hierarchy->appendRow(parent1);
    parent1->appendRow(new QStandardItem(QStringLiteral("bar")));
    ui->treeView->setModel(hierarchy);
    
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hierarchy;
}

void MainWindow::on_actionopen_triggered()
{
    // 菜单点击事件
    statusBar()->showMessage("actionopen");
}
