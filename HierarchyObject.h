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


    // �Ӿֲ�����ת��������
    glm::mat4 localToWorld();
    // ����������ת�ֲ�����
    glm::mat4 worldToLocal() {
        return glm::inverse(localToWorld());
    }
    
    // ������״�ṹ
    void setParent(HierarchyObject* newParent);

    // ��ȡ��i�������cast��ָ������
    template <class T>
    T* getComponent(int i) {
        return dynamic_cast<T*>(components[i]);
    }

    // ��ȡ��һ��ָ�����͵����
    template <class T>
    T* getComponent() {
        for (auto component : components) {
            if (dynamic_cast<T*>(component)) {
                return dynamic_cast<T*>(component);
            }
        }
        return NULL;
    }

    // ������
    void addComponent(Component* component);

    // ��������
    void updateRecursively();

    // �ݻ�����
    void Destroy();

};


