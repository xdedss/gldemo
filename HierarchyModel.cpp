#include "HierarchyModel.h"
#include "HierarchyObject.h"
#include "widget.h"
#include "qapplication.h"
#include "qmimedata.h"

HierarchyModel::HierarchyModel()
{
    root = new HierarchyObject("root");
}

HierarchyObject * HierarchyModel::createObject(const QString& name)
{
    Q_ASSERT(widget); // 此时必须已经添加到widget上  

    // 默认挂到root上  
    beginInsertRows(QModelIndex(), root->childrenCount(), root->childrenCount() + 1);

    HierarchyObject * res = new HierarchyObject(name);
    //res->widget = widget;
    //res->hierarchy = this;
    objects.push_back(res);
    root->insertChild(root->childrenCount(), res);

    endInsertRows();
    return res;
}

void HierarchyModel::moveObject(HierarchyObject* obj, HierarchyObject* toParent, int toIndex) {
    Q_ASSERT(obj->getParent() && toParent);
    Q_ASSERT(toIndex >= 0 && toIndex <= toParent->childrenCount());
    
    if (obj->getParent() == toParent) {
        // 同级移动  
        //assert(toIndex < toParent->childrenCount());
        int fromIndex = toParent->findChild(obj);
        int realToIndex = (fromIndex < toIndex) ? (toIndex - 1) : toIndex; // 目标在后，向前挪一个  
        if (fromIndex == realToIndex) {
            // 不用移  
            return;
        }
        //QModelIndex changeBegin = obj2index(obj), changeEnd = obj2index(toParent->getChildren(toIndex));
        // https://bugreports.qt.io/browse/QTBUG-6940
        QModelIndex parentIndex = obj2index(toParent);
        beginMoveRows(parentIndex, fromIndex, fromIndex, parentIndex, toIndex);
        toParent->moveChild(fromIndex, realToIndex);
        endMoveRows();
        //emit(dataChanged(changeBegin, changeEnd, { Qt::EditRole }));
    }
    else {
        // 移动到别的子树上  
        // 先移除，再添加  
        QModelIndex originalParentIndex = obj2index(obj->getParent());
        int objIndex = obj->getParent()->findChild(obj);
        beginRemoveRows(originalParentIndex, objIndex, objIndex);
        obj->getParent()->popChild(objIndex);
        endRemoveRows();

        // 添加  
        QModelIndex toParentIndex = obj2index(toParent);
        beginInsertRows(toParentIndex, toIndex, toIndex);
        toParent->insertChild(toIndex, obj);
        endInsertRows();

    }
}

void HierarchyModel::removeObject(HierarchyObject* obj) {
    QModelIndex originalParentIndex = obj2index(obj->getParent());
    int objIndex = obj->getParent()->findChild(obj);
    beginRemoveRows(originalParentIndex, objIndex, objIndex);
    obj->getParent()->popChild(objIndex);
    endRemoveRows();
}

HierarchyObject* HierarchyModel::index2obj(const QModelIndex & index) const
{
    HierarchyObject *res;
    if (!index.isValid()) {
        res = root;
    }
    else {
        res = static_cast<HierarchyObject*>(index.internalPointer());
    }
    return res;
}

QModelIndex HierarchyModel::obj2index(HierarchyObject* obj) const
{
    assert(obj);
    if (obj == root) return QModelIndex();
    assert(obj->getParent());
    return createIndex(obj->getParent()->findChild(obj), 0, obj);
}

void HierarchyModel::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    QModelIndexList selectedIndices = selected.indexes();
    QModelIndexList deselectedIndices = deselected.indexes();
    // 取消所有高亮  
    root->callRecursively([](HierarchyObject* o) -> void {
        Renderer* renderer = o->getComponent<Renderer>();
        if (renderer != NULL) {
            renderer->highlight = false;
        }
    });
    // 重新高亮  
    for (auto& index : selectedIndices) {
        HierarchyObject* obj = index2obj(index);
        assert(obj);
        obj->callRecursively([](HierarchyObject* o) -> void {
            Renderer* renderer = o->getComponent<Renderer>();
            if (renderer != NULL) {
                renderer->highlight = true;
            }
        });
    }
}


// ---------abstract item model--------------------

QModelIndex HierarchyModel::index(int row, int column, const QModelIndex & parent) const
{
    HierarchyObject *parentItem = index2obj(parent);
    HierarchyObject *childItem = parentItem->getChildren(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex HierarchyModel::parent(const QModelIndex & index) const
{
    if (!index.isValid()) //传入节点是根节点  
        return QModelIndex();


    HierarchyObject *childItem = static_cast<HierarchyObject*>(index.internalPointer());
    HierarchyObject *parentItem = childItem->getParent();

    if (parentItem == root)
        return QModelIndex();

    return createIndex(parentItem->getParent()->findChild(parentItem), 0, parentItem);
}

int HierarchyModel::rowCount(const QModelIndex & parent) const
{
    HierarchyObject * parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<HierarchyObject*>(parent.internalPointer());

    return parentItem->childrenCount();
}

int HierarchyModel::columnCount(const QModelIndex & parent) const
{
    return 1;
}

QVariant HierarchyModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    HierarchyObject *item = static_cast<HierarchyObject*>(index.internalPointer());
    assert(item);
    return item->name;
}

bool HierarchyModel::setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {
    qDebug() << "setData: " << value << "|" << role;

    HierarchyObject * item;

    if (!index.isValid()) return false;
    else {
        item = static_cast<HierarchyObject*>(index.internalPointer());
        item->name = value.toString();
        emit(dataChanged(index, index, { role }));
        return true;
    }
    
}

Qt::ItemFlags HierarchyModel::flags(const QModelIndex & index) const
{
    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

static const char s_treeNodeMimeType[] = "application/x-treenode";

bool HierarchyModel::canDropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent) const
{
    if (!data->hasFormat(s_treeNodeMimeType)) {
        return false;
    }
    //std::vector<HierarchyObject*> droppedNodes;
    //// 获取失败，不是同一个进程
    //if (!retrieveDroppedObjects(data, droppedNodes)) {
    //    return false;
    //}
    //// 拖放目标
    //HierarchyObject *parentNode = index2obj(parent);
    //Q_ASSERT(parentNode);

    // 经测试已经自带了判断是不是拖到自己的子树的功能
    // 暂时不用加别的检测

    return true;
}

QMimeData * HierarchyModel::mimeData(const QModelIndexList & indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray data; //a kind of RAW format for datas

    QDataStream stream(&data, QIODevice::WriteOnly);
    QList<HierarchyObject *> nodes;

    // 取出所有选中项的指针
    foreach(const QModelIndex &index, indexes) {
        HierarchyObject *node = index2obj(index);
        if (!nodes.contains(node))
            nodes << node;
    }
    // 写入进程pid
    stream << QCoreApplication::applicationPid();
    // 写入选中项数量
    stream << nodes.count();
    // 写入选中项指针
    foreach(HierarchyObject *node, nodes) {
        stream << reinterpret_cast<qlonglong>(node);
    }
    mimeData->setData(s_treeNodeMimeType, data);
    return mimeData;
}

bool HierarchyModel::retrieveDroppedObjects(const QMimeData * data, std::vector<HierarchyObject*>& out_objects) const {
    QByteArray bytes = data->data(s_treeNodeMimeType);
    QDataStream stream(&bytes, QIODevice::ReadOnly);// 准备读取
    qint64 senderPid;
    stream >> senderPid; // 读出进程pid
    if (senderPid != QCoreApplication::applicationPid()) {
        // Let's not cast pointers that come from another process...
        return false;
    }
    int count;
    stream >> count; // 读出拖入项数量
    // 以longlong读出指针并转换回来
    for (int i = 0; i < count; ++i) {
        // Decode data from the QMimeData
        qlonglong nodePtr;
        stream >> nodePtr;
        HierarchyObject* objPoiner = reinterpret_cast<HierarchyObject *>(nodePtr);
        assert(objPoiner);
        out_objects.push_back(objPoiner);
    }
    return true;
}

// https://forum.qt.io/topic/76708/full-drag-and-drop-support-in-qtreeview/3
bool HierarchyModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
    Q_ASSERT(action == Qt::MoveAction);
    Q_UNUSED(column);
    //test if the data type is the good one
    if (!data->hasFormat(s_treeNodeMimeType)) {
        return false;
    }
    std::vector<HierarchyObject*> droppedNodes;
    // 获取失败，不是同一个进程
    if (!retrieveDroppedObjects(data, droppedNodes)) {
        return false;
    }
    // 拖放目标
    HierarchyObject *parentNode = index2obj(parent);
    Q_ASSERT(parentNode);

    // 处理行数的特殊情形
    if (row == -1) {
        // valid index means: drop onto item. -> insert at begin
        if (parent.isValid()) row = 0;
        // invalid index means: append at bottom, after last toplevel
        else  row = rowCount(parent);
    }

    qDebug() << "parent: " << parentNode->name << " row" << row << "  mime contents: " << droppedNodes.size();
    for (auto objPoiner : droppedNodes) {
        qDebug() << objPoiner->name;
    }

    for (auto& droppedNode : droppedNodes) {
        moveObject(droppedNode, parentNode, row);
    }
    return true;
}

QStringList HierarchyModel::mimeTypes() const
{
    return QStringList() << s_treeNodeMimeType;
}

Qt::DropActions HierarchyModel::supportedDropActions() const
{
    return Qt::MoveAction;
}


QVariant HierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant(QString::fromUtf8("sdf啊 "));
}

