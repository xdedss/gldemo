#pragma once


#include <qpoint.h>
#include <qstring.h>

#include <vector>
#include <set>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <queue>

#include "nesteddefs.h"


class HierarchyObject {

private:
    std::vector<HierarchyObject*> children;
    std::vector<Component*> components;
    HierarchyObject* parent = NULL;

public:
    glm::mat4 transform;
    //Widget* widget;
    //HierarchyModel* hierarchy;
    QString name;

    // 子节点数量
    int childrenCount() { return children.size(); }
    // 获取第i个子节点
    HierarchyObject* getChildren(int i) { return (children.size() > i && i >= 0) ? children[i] : NULL; }
    // 寻找指定名称子节点
    HierarchyObject* getChildren(const QString& name);
    // 找到子节点序号
    int findChild(HierarchyObject* child);
    // 本节点挂载的组件数量
    int componentsCount() { return components.size(); }
    // 获取父节点
    HierarchyObject* getParent() { return parent; }

    // ctor
    HierarchyObject(const QString& name, HierarchyObject* parent = NULL);


    // 从局部坐标转世界坐标
    glm::mat4 localToWorld();
    // 从世界坐标转局部坐标
    glm::mat4 worldToLocal() {
        return glm::inverse(localToWorld());
    }
    
    // 以下不应单独调用，应该调用HierarchyModel中的相应方法
    //// 更改父节点（仅调整，未通知treeview，不应被单独调用）
    //void setParent(HierarchyObject* newParent);
    // 取出子节点（仅调整，未通知treeview，不应被单独调用）
    HierarchyObject* popChild(int index);
    // 放入子节点（仅调整，未通知treeview，不应被单独调用）
    void insertChild(int index, HierarchyObject* child);
    // 调整子节点顺序（仅调整，未通知treeview，不应被单独调用）
    void moveChild(int oldIndex, int newIndex);

    // 获取第i个组件，cast成指定类型
    template <class T>
    T* getComponent(int i) {
        if (i < 0 || i >= components.size()) return NULL;
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
    Component * addComponent(Component* component);


    // 更新子树
    void updateRecursively();

    // 遍历子树
    void callRecursively(const std::function<void(HierarchyObject*)>& func);

    // 从场景移除
    void Remove();

    // 摧毁子树
    void Destroy();

};


