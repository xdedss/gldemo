#include "HierarchyObject.h"
#include "HierarchyModel.h"
#include "Component.h"
#include "qdebug.h"

HierarchyObject * HierarchyObject::getChildren(const QString & name)
{
    for (auto child : children) {
        if (child->name == name) {
            return child;
        }
    }
    return nullptr;
}

int HierarchyObject::findChild(HierarchyObject * child)
{
    for (int i = 0; i < children.size(); i++) {
        if (child == children[i]) return i;
    }
    return -1;
}

HierarchyObject::HierarchyObject(const QString & name, HierarchyObject* parent)
{
    transform = glm::identity<glm::mat4>();
    this->name = name;
    this->parentObj = parent;
    this->enabled = true;
}

glm::mat4 HierarchyObject::localToWorld()
{
    glm::mat4 res = glm::identity<glm::mat4>();
    HierarchyObject* nextNode = this;
    while (nextNode) {
        res = nextNode->transform * res;
        nextNode = nextNode->getParent();
    }
    return res;
}

//void HierarchyObject::setParent(HierarchyObject * newParent)
//{
//    //assert(parent != NULL);
//    if (parent != NULL) { // 初始添加时会出现parent==NULL
//        std::vector<HierarchyObject*>& tempchildren = parent->children;
//        tempchildren.erase(std::find(tempchildren.begin(), tempchildren.end(), this));
//    }
//    if (newParent == NULL) newParent = hierarchy->root;
//    parent = newParent;
//    newParent->children.push_back(this);
//}

HierarchyObject* HierarchyObject::popChild(int index) {
    Q_ASSERT(index < children.size() && index >= 0);
    HierarchyObject* child = children[index];
    children.erase(children.begin() + index);
    child->parentObj = NULL;
    return child;
}

void HierarchyObject::insertChild(int index, HierarchyObject* child) {
    Q_ASSERT(index <= children.size() && index >= 0);
    children.insert(children.begin() + index, child);
    child->parentObj = this;
}

void HierarchyObject::moveChild(int oldIndex, int newIndex) {
    Q_ASSERT(oldIndex >= 0 && oldIndex < children.size());
    Q_ASSERT(newIndex >= 0 && newIndex < children.size());
    auto& v = children;
    if (oldIndex > newIndex)
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
    else
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

Component * HierarchyObject::addComponent(Component * component)
{
    assert(component);
    components.push_back(component);
    component->hierarchyObject = this;
    component->onInit();
    return component;
}

void HierarchyObject::updateRecursively()
{
    if (!enabled) return;
    for (auto component : components) {
        assert(component);
        if (component->getProp("enabled").toBool()) {
            component->onUpdate();
        }
    }
    for (auto child : children) {
        assert(child);
        child->updateRecursively();
    }
}

void HierarchyObject::callRecursively(const std::function<void(HierarchyObject*)>& func, bool requireEnabled)
{
    std::queue<HierarchyObject*> todoList;
    todoList.push(this);
    int failsafe = 0;
    while (todoList.size() > 0) {
        auto obj = todoList.front();
        if ((!requireEnabled) || obj->enabled) {
            for (auto child : obj->children) {
                todoList.push(child);
            }
            func(obj);
        }
        todoList.pop();
        if (++failsafe > 100000) {
            throw "this must be broken";
        }
    }
}



// ---------abstract item model--------------------


QModelIndex HierarchyObject::index(int row, int column, const QModelIndex & parent) const
{
    if (!parent.isValid()) {
        // 根节点index，返回component标题项   
        if (row >= components.size() || row < 0) return QModelIndex();
        return createIndex(row, column, nullptr);
    }
    else {
        // 根节点就是component，返回属性项  
        if (parent.row() >= components.size() || parent.row() < 0) return QModelIndex();
        return createIndex(row, column, components[parent.row()]);
    }
}

QModelIndex HierarchyObject::parent(const QModelIndex & index) const
{
    if (!index.isValid()) //传入节点是根节点  
        return QModelIndex();

    if (index.internalPointer() == nullptr) {
        // 是component
        return QModelIndex();
    }

    // 剩余情况一定是属性项  
    Component* p = static_cast<Component*>(index.internalPointer());
    int row = std::find(components.begin(), components.end(), p) - components.begin();
    return createIndex(row, 0, nullptr);
}

int HierarchyObject::rowCount(const QModelIndex & parent) const
{
    if (!parent.isValid()) //传入节点是根节点  
        return components.size();

    if (parent.internalPointer() == nullptr) {
        // 是component
        return components[parent.row()]->getPropKeys().size();
    }
    // property本身没有子节点  
    return 0;
}

int HierarchyObject::columnCount(const QModelIndex & parent) const
{
    if (!parent.isValid()) //传入节点是根节点  
        return 1;

    if (parent.internalPointer() == nullptr) {
        // 是component， 下面的property有两列  
        return 2;
    }
    // property  
    return 0;
}

QVariant HierarchyObject::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) //传入节点是根节点  
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    if (index.internalPointer() == nullptr) {
        // 是component 返回其名称   
        return QVariant(components[index.row()]->name);
    }
    // property  
    Component* c = static_cast<Component*>(index.internalPointer());
    if (index.column() == 0) {
        // 返回属性名   
        return c->getPropKeys()[index.row()];
    }
    else {
        // 返回属性值   
        return c->getProp(c->getPropKeys()[index.row()]);
    }
}

HierarchyObject::~HierarchyObject()
{
    for (auto component : components) {
        assert(component);
        delete component;
    }
    for (auto child : children) {
        assert(child);
        delete child;
    }
}

bool HierarchyObject::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid()) return false;

    if (index.internalPointer() == nullptr) {
        // 是component 不允许修改    
        return false;
    }
    // property  
    Component* c = static_cast<Component*>(index.internalPointer());
    if (index.column() == 0) {
        // 属性名不允许修改   
        return false;
    }
    else {
        // 修改属性值
        qDebug() << "setData: " << value << "|" << role;
        c->setProp(c->getPropKeys()[index.row()], value);
        return true;
    }
}

Qt::ItemFlags HierarchyObject::flags(const QModelIndex & index) const
{
    if (!index.isValid()) return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (index.internalPointer() == nullptr) {
        // 是component 不允许修改    
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    // property  
    if (index.column() == 0) {
        // 属性名不允许修改   
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else {
        // 属性值
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

QVariant HierarchyObject::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant(QString("123"));
}
