#pragma once


#include <qpoint.h>
#include <qstring.h>

#include <vector>
#include <set>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "nesteddefs.h"


class HierarchyObject {

private:
    std::vector<HierarchyObject*> children;
    std::vector<Component*> components;
    HierarchyObject* parent = NULL;

public:
    glm::mat4 transform;
    Widget* widget;
    QString name;

    int childrenCount() { return children.size(); }
    HierarchyObject* getChildren(int i) { return children[i]; }
    int componentsCount() { return components.size(); }
    HierarchyObject* getParent() { return parent; }

    HierarchyObject(const QString& name);


    // 从局部坐标转世界坐标
    glm::mat4 localToWorld();
    // 从世界坐标转局部坐标
    glm::mat4 worldToLocal() {
        return glm::inverse(localToWorld());
    }
    
    // 更改树状结构
    void setParent(HierarchyObject* newParent);

    // 获取第i个组件，cast成指定类型
    template <class T>
    T* getComponent(int i) {
        return dynamic_cast<T*>(components[i]);
    }

    // 获取第一个指定类型的组件
    template <class T>
    T* getComponent() {
        for (auto component : components) {
            if (dynamic_cast<T*>(component)) {
                return dynamic_cast<T*>(component);
            }
        }
        return NULL;
    }

    // 添加组件
    void addComponent(Component* component);

    // 更新子树
    void updateRecursively();

    // 摧毁子树
    void Destroy();

};


