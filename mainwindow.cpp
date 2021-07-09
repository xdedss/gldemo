#include "mainwindow.h"
#include "ui_mainwindow.h"


int matchRowData(QStandardItem* item, QVariant data) {
    for (int i = 0; i < item->rowCount(); i++) {
        if (item->child(i)->data() == data) {
            return i;
        }
    }
    return -1;
}


// 导入点云
PointCloudRenderer* MainWindow::importPointCloud(const QString& path, float initialScale = 1) {
    HierarchyObject* obj = ui->openGLWidget->hierarchy->createObject(path.split(QRegularExpression("[/\\\\]")).last());
    obj->transform = glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * initialScale);
    PointCloudRenderer* renderer = new PointCloudRenderer();
    obj->addComponent(renderer);

    if (path.endsWith(".ply")) {
        auto vertices = readPly(path.toStdString());
        renderer->setVertices(vertices);
    }
    else if (path.endsWith(".txt")) {
        renderer->setVertices(readTxt(path.toStdString()));
    }
    else {
        throw "file format not supported";
    }
    //ui->openGLWidget->pointClouds.push_back(pointCloud);

    //QStandardItem* hierarchyItem = new QStandardItem(path.split(QRegularExpression("[/\\\\]")).last());
    //hierarchyItem->setData((intptr_t)pointCloud);
    //modelsParent->appendRow(hierarchyItem);

    return renderer;
}

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
    ui->splitter_hor->setStretchFactor(2, 1);

    
    // 左侧树状结构 以后可以做成和场景内容相关联
    QStringList thead;
    thead << "Hierarchy";
    hierarchy = new QStandardItemModel();
    hierarchy->setHorizontalHeaderLabels(thead);
    modelsParent = new QStandardItem(QString::fromUtf8("模型"));
    modelsParent->setEditable(false);
    modelsParent->setSelectable(false);
    trailsParent = new QStandardItem(QString::fromUtf8("轨迹"));
    trailsParent->setEditable(false);
    trailsParent->setSelectable(false);
    hierarchy->appendRow(modelsParent);
    hierarchy->appendRow(trailsParent);
    // 自动展开
    connect(hierarchy, &QAbstractItemModel::rowsInserted, [this](const QModelIndex &parent, int first, int last)
    {
        if (!ui->treeView_hierarchy->isExpanded(parent)) {
            ui->treeView_hierarchy->expand(parent);
        }
    });
    // 加入treeview
    qDebug() << modelsParent->index();
    ui->treeView_hierarchy->setExpanded(modelsParent->index(), true);

    ui->treeView_hierarchy->setModel(hierarchy);
    ui->treeView_hierarchy->header()->setSectionResizeMode(QHeaderView::Stretch);

    // 右侧属性面板
    QStringList thead2;
    thead2 << "Properties" << "Values";
    QStandardItemModel* propModel = new QStandardItemModel();
    propModel->setHorizontalHeaderLabels(thead2);
    QStandardItem* parent1 = new QStandardItem(QStringLiteral("foo"));
    propModel->appendRow(parent1);
    QStandardItem* parent2 = new QStandardItem(QStringLiteral("foo2"));
    parent2->setEditable(false);
    propModel->appendRow(parent2);
    propModel->appendRow({ new QStandardItem("sdf"), new QStandardItem("wwww1") });
    // 加入treeview
    ui->treeView_prop->setModel(propModel);
    ui->treeView_prop->header()->setSectionResizeMode(QHeaderView::Stretch);

    ui->treeView_prop->setEnabled(false);

    // 测试：加载模型
    ui->openGLWidget->setHierarchy(new HierarchyModel());

    auto bun = importPointCloud("bun180.ply", 10);
    auto building = importPointCloud("uwo.txt");
    building->hierarchyObject->transform = glm::rotate(
        glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * 0.1f),
        3.5f, { 1.0f, 0.0f, 0.0f });
    
    

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
