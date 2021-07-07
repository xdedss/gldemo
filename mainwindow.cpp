#include "mainwindow.h"
#include "ui_mainwindow.h"

// �����ڳ�ʼ��
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Done");

    // �ָ���Ĭ�ϱ���
    ui->splitter_hor->setStretchFactor(0, 1);
    ui->splitter_hor->setStretchFactor(1, 3);

    // �����״�ṹ �Ժ�������ɺͳ������������
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
    // �˵�����¼�
    statusBar()->showMessage("actionopen");
}
