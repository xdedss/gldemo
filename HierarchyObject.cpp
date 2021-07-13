#include "HierarchyObject.h"
#include "HierarchyModel.h"
#include "Component.h"

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
    this->parent = parent;
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
    child->parent = NULL;
    return child;
}

void HierarchyObject::insertChild(int index, HierarchyObject* child) {
    Q_ASSERT(index <= children.size() && index >= 0);
    children.insert(children.begin() + index, child);
    child->parent = this;
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
        component->onUpdate();
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

void HierarchyObject::Remove() {
    throw "not implemented";
}

void HierarchyObject::Destroy()
{
    for (auto component : components) {
        assert(component);
        delete component;
    }
    for (auto child : children) {
        assert(child);
        child->Destroy();
    }
}
