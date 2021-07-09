#include "HierarchyObject.h"
#include "HierarchyModel.h"
#include "Component.h"

int HierarchyObject::findChild(HierarchyObject * child)
{
    for (int i = 0; i < children.size(); i++) {
        if (child == children[i]) return i;
    }
    return -1;
}

HierarchyObject::HierarchyObject(const QString & name)
{
    transform = glm::identity<glm::mat4>();
    this->name = name;
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

void HierarchyObject::setParent(HierarchyObject * newParent)
{
    if (parent != NULL) {
        std::vector<HierarchyObject*>& tempchildren = parent->children;
        tempchildren.erase(std::find(tempchildren.begin(), tempchildren.end(), this));
    }
    if (newParent == NULL) newParent = hierarchy->root;
    parent = newParent;
    newParent->children.push_back(this);
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
    for (auto component : components) {
        assert(component);
        component->onUpdate();
    }
    for (auto child : children) {
        assert(child);
        child->updateRecursively();
    }
}

void HierarchyObject::callRecursively(const std::function<void(HierarchyObject*)>& func)
{
    std::queue<HierarchyObject*> todoList;
    todoList.push(this);
    int failsafe = 0;
    while (todoList.size() > 0) {
        auto obj = todoList.front();
        for (auto child : obj->children) {
            todoList.push(child);
        }
        func(obj);
        todoList.pop();
        if (++failsafe > 100000) {
            throw "this must be broken";
        }
    }
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
