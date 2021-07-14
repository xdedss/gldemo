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
    HierarchyObject* obj = hierarchy->createObject(path.split(QRegularExpression("[/\\\\]")).last());
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

// 插入keypoint   
void addKeypoint(HierarchyObject* obj, HierarchyModel* hierarchy, float angle, glm::vec3 axis, glm::vec3 translation) {
    auto kp = hierarchy->createObject("keypoint");
    kp->transform = glm::translate(glm::identity<glm::mat4>(), translation)
        * glm::toMat4(glm::angleAxis(angle, axis));
    hierarchy->moveObject(kp, obj, obj->childrenCount());
}

// 主窗口初始化
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    statusBar()->showMessage("Done");

    connect(ui->openGLWidget, SIGNAL(onSelection(HierarchyObject*)), this, SLOT(onWidgetSelection(HierarchyObject*)));

    // 分割线默认比例  
    ui->splitter_hor->setStretchFactor(0, 1);
    ui->splitter_hor->setStretchFactor(1, 3);
    ui->splitter_hor->setStretchFactor(2, 1);

    

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

    // 初始化场景  
    hierarchy = new HierarchyModel();
    ui->openGLWidget->setHierarchy(hierarchy);
    ui->treeView_hierarchy->setModel(hierarchy);
    ui->treeView_hierarchy->header()->setSectionResizeMode(QHeaderView::Stretch);
    // 初始化菜单  
    ui->treeView_hierarchy->setContextMenuPolicy(Qt::CustomContextMenu);
    treeContextMenu = new QMenu(ui->openGLWidget);
    treeContextMenu->addAction("add child", this, SLOT(onTreeViewAddObject()));
    treeContextMenu->addAction("delete", this, SLOT(onTreeViewRemoveObject()));
    treeContextMenuSpace = new QMenu(ui->openGLWidget);
    treeContextMenuSpace->addAction("add child", this, SLOT(onTreeViewAddObject()));
    // 链接选择信号槽  
    connect(ui->treeView_hierarchy->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
        hierarchy, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));


    connect(ui->treeView_hierarchy->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(ObjectSelected(const QItemSelection&, const QItemSelection&)));
    // 链接右键菜单信号槽  
    connect(ui->treeView_hierarchy, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onTreeViewCustomContextMenu(const QPoint &)));

    // 测试用：加载模型  
    auto buildingRoot = hierarchy->createObject("building");
    buildingRoot->transform = glm::rotate(
        glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * 0.1f),
        3.5f, { 1.0f, 0.0f, 0.0f });
    auto bun = importPointCloud("bun180.ply", 10);
    bun->sizeScale = 2;
    LineRenderer* l1 = new LineRenderer();
    
    //l1->setVertices({ {{0,0,0},{0,0,0}} });
    bun->hierarchyObject->addComponent(l1);

    LineRenderer* l2 = new LineRenderer();
    //l2->setVertices({ {{0,0,0},{0,0,0}} });
    auto building = importPointCloud("uwo.txt");
    building->hierarchyObject->addComponent(l2);
    building->sizeScale = 2;
    hierarchy->moveObject(building->hierarchyObject, buildingRoot, 0);

    auto trailTest = hierarchy->createObject("trailTest");
    trailTest->addComponent(new Trail());
    addKeypoint(trailTest, hierarchy, 0.0f, { 0, 1, 0 }, { 0, 0, 10 });
    addKeypoint(trailTest, hierarchy, 0.0f, { 0, 1, 0 }, { 0, 1, 5 });
    addKeypoint(trailTest, hierarchy, 3.14159 / 4, { 0, 1, 0 }, { 3, 1, 3 });
    addKeypoint(trailTest, hierarchy, 3.14159 / 2, { 0, 1, 0 }, { 4, 0.5, -1 });
    addKeypoint(trailTest, hierarchy, 3.14159 / 2 + 0.5, { 0, 1, 0 }, { 2, 0.5, -5 });
    
    
    //ui->treeView_hierarchy->addAction(ui->actionbar);
    //connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(btn_slot1()));
    //connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(btn_slot2()));
    //connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(btn_slot3()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete hierarchy->root;
    delete hierarchy;
}


void MainWindow::ObjectSelected(const QItemSelection& selected, const QItemSelection& deselected) {
    QModelIndexList selectedIndices = selected.indexes();
    QModelIndexList deselectedIndices = deselected.indexes();  
    HierarchyObject* obj = hierarchy->index2obj(selectedIndices[selectedIndices.count()-1]);
}

//void MainWindow::btn_slot1()
//{
//
//    qDebug() << ui->lineEdit->text();
//    qDebug() << ui->lineEdit_2->text();
//    qDebug() << ui->lineEdit_3->text();
//}
//void MainWindow::btn_slot2()
//{
//    float scalex, scaley, scalez;
//    scalex = QString(ui->lineEdit_6->text()).toFloat();
//    scaley = QString(ui->lineEdit_7->text()).toFloat();
//    scalez = QString(ui->lineEdit_10->text()).toFloat();
//    //scaleChange(scalex, scaley, scalez, NULL);
//    qDebug() << ui->lineEdit_6->text();
//    qDebug() << ui->lineEdit_7->text();
//    qDebug() << ui->lineEdit_10->text();
//}
//void MainWindow::btn_slot3()
//{
//
//    qDebug() << ui->lineEdit_8->text();
//    qDebug() << ui->lineEdit_9->text();
//    qDebug() << ui->lineEdit_11->text();
//}
void MainWindow::onEdited()
{
    // 编辑完按下回车了   

    qDebug() << "www";
    float scalex, scaley, scalez;
    scalex = QString(ui->lineEdit_6->text()).toFloat();
    scaley = QString(ui->lineEdit_7->text()).toFloat();
    scalez = QString(ui->lineEdit_10->text()).toFloat();
    scaleChange(scalex, scaley, scalez, nullptr);

}
void MainWindow::scaleChange(float scalex, float scaley, float scalez, HierarchyObject*&obj) {
    if (obj) {
        obj->transform *= glm::mat4({ { scalex,0,0,0 }, { scalex,0,0,0 }, { scalex,0,0,0 }, {0,0,0,1} });
    }
}



void MainWindow::onTreeViewCustomContextMenu(const QPoint & point) {
    QModelIndex index = ui->treeView_hierarchy->indexAt(point);
    hierarchy->lastRightClick = index;
    if (index.isValid()) {
        treeContextMenu->exec(ui->treeView_hierarchy->viewport()->mapToGlobal(point));
    }
    else {
        treeContextMenuSpace->exec(ui->treeView_hierarchy->viewport()->mapToGlobal(point));
    }
}

void MainWindow::onTreeViewAddObject() {
    auto obj = hierarchy->createObject("new Obj");
    auto parent = hierarchy->index2obj(hierarchy->lastRightClick);
    hierarchy->moveObject(obj, parent, parent->childrenCount());
}

void MainWindow::onTreeViewRemoveObject() {
    auto obj = hierarchy->index2obj(hierarchy->lastRightClick);
    hierarchy->removeObject(obj);
    //auto obj = hierarchy->createObject("new Obj");

    //QModelIndex index = QModelIndex(-1, -1, nullptr, hierarchy);

    //emit(hierarchy->dataChanged(index, index));
}

void MainWindow::onWidgetSelection(HierarchyObject * obj)
{
    if (obj) {
        ui->treeView_hierarchy->selectionModel()->select(hierarchy->obj2index(obj), QItemSelectionModel::ClearAndSelect);
    }
    else {
        ui->treeView_hierarchy->selectionModel()->clearSelection();
    }
}

void MainWindow::on_actionopen_triggered()
{
    // 菜单点击事件  
    statusBar()->showMessage("actionopen");
}
