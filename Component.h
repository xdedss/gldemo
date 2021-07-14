#pragma once


#include "nesteddefs.h"
#include "qvariant.h"
#include "qabstractitemmodel.h"

// 组件的基类。里面的事件会由widget调用   
class Component {

protected:
    std::map<QString, QVariant> properties;

public:
    HierarchyObject* hierarchyObject;

    virtual void onInit() {};
    virtual void onUpdate() {};

public:
    QVariant getProp(const QString& key);
    void setProp(const QString& key, QVariant value);


};



