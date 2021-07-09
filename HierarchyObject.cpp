#include "HierarchyObject.h"
#include "HierarchyModel.h"
#include "Component.h"

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
    parent = newParent;
    if (newParent != NULL) {
        newParent->children.push_back(this);
    }
}

void HierarchyObject::addComponent(Component * component)
{
    assert(component);
    components.push_back(component);
    component->hierarchyObject = this;
    component->onInit();
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
