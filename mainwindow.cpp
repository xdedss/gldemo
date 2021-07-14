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
    thead2 << "Property" << "Value";
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
	// 链接拖拽生成点云信号槽  
	connect(ui->openGLWidget, SIGNAL(drag_signal(std::string)), this, SLOT(drag_solt(std::string)));

    // 测试用：加载模型   
    auto buildingRoot = hierarchy->createObject("building");
    buildingRoot->transform = glm::rotate(
        glm::scale(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1) * 0.1f),
        3.5f, { 1.0f, 0.0f, 0.0f });
    auto bun = importPointCloud("bun180.ply", 10);
    bun->setProp("sizeScale", 2.0f);
    LineRenderer* l1 = new LineRenderer();
    
    //l1->setVertices({ {{0,0,0},{0,0,0}} });
    bun->hierarchyObject->addComponent(l1);

    LineRenderer* l2 = new LineRenderer();
    //l2->setVertices({ {{0,0,0},{0,0,0}} });
    auto building = importPointCloud("uwo.txt");
    building->hierarchyObject->addComponent(l2);
    building->setProp("sizeScale", 2.0f);
    hierarchy->moveObject(building->hierarchyObject, buildingRoot, 0);

    auto trailTest = hierarchy->createObject("trailTest");
    trailTest->addComponent(new Trail());
    addKeypoint(trailTest, hierarchy, 0.0f, { 0, 1, 0 }, { 0, 0, 10 });
    addKeypoint(trailTest, hierarchy, 0.0f, { 0, 1, 0 }, { 0, 1, 5 });
    addKeypoint(trailTest, hierarchy, 3.14159 / 4, { 0, 1, 0 }, { 3, 1, 3 });
    addKeypoint(trailTest, hierarchy, 3.14159 / 2, { 0, 1, 0 }, { 4, 0.5, -1 });
    addKeypoint(trailTest, hierarchy, 3.14159 / 2 + 0.5, { 0, 1, 0 }, { 2, 0.5, -5 });
    addKeypoint(trailTest, hierarchy, 3.14159 * 3 / 4, { 0, 1, 0 }, { 0, 0.5, -5 });
    
    
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
    HierarchyObject* obj = hierarchy->lastSelected;
    if (obj) {
        qDebug() << obj->name;
        glm::vec3 scale,translation,skew;
        glm::vec4 perspective;
        glm::quat orientation;
        glm::decompose(obj->transform, scale, orientation, translation, skew, perspective);
        glm::vec3 rotation = glm::eulerAngles(orientation);
        glm::mat3 rotationMatrix_inv = glm::toMat3(orientation);
        //歪打正着，读取了某列的某行，即读取了矩阵的逆
        qDebug() << rotationMatrix_inv[0][0] << rotationMatrix_inv[1][0] << rotationMatrix_inv[2][0];
        qDebug() << rotationMatrix_inv[0][1] << rotationMatrix_inv[1][1] << rotationMatrix_inv[2][1];
        qDebug() << rotationMatrix_inv[0][2] << rotationMatrix_inv[1][2] << rotationMatrix_inv[2][2];
        


        ///////////
        //旋转顺序y x z    
        //x轴旋转角度       
        float eps = 0.00001;
        if (abs(rotationMatrix_inv[2][1]) < eps)
            rotation.x = 0;
        else
            rotation.x = asinf(-1 * rotationMatrix_inv[2][1]);
        float cos_x = sqrt(1 - pow(rotationMatrix_inv[2][1], 2));
        //y、z轴旋转角度    
        if (cos(rotation.x) > eps) {//cos（Rx）！=0    
            if (abs(rotationMatrix_inv[2][2]/cos_x ) > eps) {
                float rotationy = atanf(rotationMatrix_inv[2][0] / rotationMatrix_inv[2][2]);
                rotation.y = rotationy;
                if (abs(rotation.y) < eps) {
                    if (rotationMatrix_inv[2][2] / cos_x < 0)
                        rotation.y = glm::pi<float>();
                    else
                        rotation.y = 0;
                }
                else if (rotationMatrix_inv[2][2]  < 0) {
                    if (rotationMatrix_inv[2][0] < 0)
                        rotation.y = -1*glm::pi<float>() + rotation.y;
                    else if(rotationMatrix_inv[2][0] > 0)
                        rotation.y = glm::pi<float>() + rotation.y;
                }
                    
            }
            else {
                if (rotationMatrix_inv[2][0] < 0)
                    rotation.y = -1*glm::pi<float>() / 2.0f;
                else
                    rotation.y = glm::pi<float>() / 2.0f;
            }
            if (abs(rotationMatrix_inv[1][1] / cos_x) > eps) {
                float rotationz = atanf(rotationMatrix_inv[0][1] / rotationMatrix_inv[1][1]);
                rotation.z = rotationz;
                if (abs(rotation.z) < eps) {
                    if (rotationMatrix_inv[1][1] / cos_x < 0)
                        rotation.z = glm::pi<float>();
                    else
                        rotation.z = 0;
                }
                else if (rotationMatrix_inv[1][1] < 0) {
                    if (rotationMatrix_inv[0][1] < 0)
                        rotation.z = -1*glm::pi<float>() + rotation.z;
                    else if(rotationMatrix_inv[0][1] >0 )
                        rotation.z = glm::pi<float>() + rotation.z;
                }
            }
            else {
                if (rotationMatrix_inv[0][1] < 0)
                    rotation.z = -1 * glm::pi<float>() / 2.0f;
                else
                    rotation.z = glm::pi<float>() / 2.0f;
            }
        }
        else {
            rotation.z = 0;
            float cos_y_z = rotationMatrix_inv[0][0], sin_y_z;
            if (-1 * rotationMatrix_inv[2][1] < 0)
                //此处代表rotationx为-pi/2
                sin_y_z = -1 * rotationMatrix_inv[1][0];
            else if (-1 * rotationMatrix_inv[2][1] > 0)
                //此处代表rotationx为 pi/2
                sin_y_z = -1 * rotationMatrix_inv[1][0];
            
            if (abs(cos_y_z) > eps) {
                rotation.y = atanf(sin_y_z / cos_y_z);
                if (cos_y_z < 0)
                    rotation.y = -1 * glm::pi<float>() + rotation.y;
                else
                    rotation.y = glm::pi<float>() + rotation.y;
            }
            else {
                if (sin_y_z > 0)
                    rotation.y = glm::pi<float>() / 2;
                else
                    rotation.y = -0.5 * glm::pi<float>();
            }
        }

        ///////////

        ui->lineEdit_translationX->setText(QString("%1").arg(translation.x));
        ui->lineEdit_translationY->setText(QString("%1").arg(translation.y));
        ui->lineEdit_translationZ->setText(QString("%1").arg(translation.z));
        ui->lineEdit_scaleX->setText(QString("%1").arg(scale.x));
        ui->lineEdit_scaleY->setText(QString("%1").arg(scale.y));
        ui->lineEdit_scaleZ->setText(QString("%1").arg(scale.z));
        ui->lineEdit_rotationX->setText(QString("%1").arg(rotation.x));
        ui->lineEdit_rotationY->setText(QString("%1").arg(rotation.y));
        ui->lineEdit_rotationZ->setText(QString("%1").arg(rotation.z));
    }
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
    qDebug() << "获取参数";
    
    float translationX, translationY, translationZ;
    float scaleX, scaleY, scaleZ;
    float rotationX, rotationY, rotationZ;
    scaleX = QString(ui->lineEdit_scaleX->text()).toFloat();
    scaleY = QString(ui->lineEdit_scaleY->text()).toFloat();
    scaleZ = QString(ui->lineEdit_scaleZ->text()).toFloat();
    rotationX = QString(ui->lineEdit_rotationX->text()).toFloat();
    rotationY = QString(ui->lineEdit_rotationY->text()).toFloat();
    rotationZ = QString(ui->lineEdit_rotationZ->text()).toFloat();
    translationX = QString(ui->lineEdit_translationX->text()).toFloat();
    translationY = QString(ui->lineEdit_translationY->text()).toFloat();
    translationZ = QString(ui->lineEdit_translationZ->text()).toFloat();


    float sin_x = sinf(rotationX), cos_x = cosf(rotationX);
    float sin_y = sinf(rotationY), cos_y = cosf(rotationY);
    float sin_z = sinf(rotationZ), cos_z = cosf(rotationZ);
    glm::mat4 t;//输错了但没完全输错
    glm::mat4 rotation = glm::mat4({
        {cos_y * cos_z + sin_x * sin_y * sin_z,cos_x * sin_z,sin_x * cos_y * sin_z - sin_y * cos_z,0},
        {sin_x * sin_y * cos_z - cos_y * sin_z,cos_x * cos_z,sin_y * sin_z + sin_x * cos_y * cos_z,0},
        {cos_x * sin_y,-1 * sin_x,cos_x * cos_y,0},
        {0,0,0,1},
        });
    glm::mat4 scale = glm::mat4({
        {scaleX,0,0,0},
        {0,scaleY,0,0},
        {0,0,scaleZ,0},
        {0,0,0,1},
        });
    glm::mat4 translation = glm::transpose(glm::mat4({
        {1,0,0,translationX},
        {0,1,0,translationY},
        {0,0,1,translationZ},
        {0,0,0,1},
        }));
    t = rotation;

    qDebug() << t[0][0] << t[0][1] << t[0][2] << t[0][3];
    qDebug() << t[1][0] << t[1][1] << t[1][2] << t[1][3];
    qDebug() << t[2][0] << t[2][1] << t[2][2] << t[2][3];
    qDebug() << t[3][0] << t[3][1] << t[3][2] << t[3][3];
    t = scale;

    qDebug() << t[0][0] << t[0][1] << t[0][2] << t[0][3];
    qDebug() << t[1][0] << t[1][1] << t[1][2] << t[1][3];
    qDebug() << t[2][0] << t[2][1] << t[2][2] << t[2][3];
    qDebug() << t[3][0] << t[3][1] << t[3][2] << t[3][3];
    t = translation;

    qDebug() << t[0][0] << t[0][1] << t[0][2] << t[0][3];
    qDebug() << t[1][0] << t[1][1] << t[1][2] << t[1][3];
    qDebug() << t[2][0] << t[2][1] << t[2][2] << t[2][3];
    qDebug() << t[3][0] << t[3][1] << t[3][2] << t[3][3];

    if (hierarchy->lastSelected) {
        hierarchy->lastSelected->transform = rotation * scale;
        t = hierarchy->lastSelected->transform;
        qDebug() << t[0][0] << t[0][1] << t[0][2] << t[0][3];
        qDebug() << t[1][0] << t[1][1] << t[1][2] << t[1][3];
        qDebug() << t[2][0] << t[2][1] << t[2][2] << t[2][3];
        qDebug() << t[3][0] << t[3][1] << t[3][2] << t[3][3];
        hierarchy->lastSelected->transform = translation*hierarchy->lastSelected->transform;
        t = hierarchy->lastSelected->transform;

        qDebug() << t[0][0] << t[0][1] << t[0][2] << t[0][3];
        qDebug() << t[1][0] << t[1][1] << t[1][2] << t[1][3];
        qDebug() << t[2][0] << t[2][1] << t[2][2] << t[2][3];
        qDebug() << t[3][0] << t[3][1] << t[3][2] << t[3][3];
    }
        
    //scaleChange(scalex, scaley, scalez, nullptr);
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

void MainWindow::drag_solt(std::string re_path)			//添加点云  
{
	qDebug() << "go" << endl;
	auto new_obj = importPointCloud(QString::fromStdString(re_path), 10);
	new_obj->setProp("sizeScale", 2.0f);
	LineRenderer* l1 = new LineRenderer();
	new_obj->hierarchyObject->addComponent(l1);
}
