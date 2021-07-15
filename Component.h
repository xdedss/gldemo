#pragma once

#include <set>

#include "nesteddefs.h"
#include "qvariant.h"
#include "qabstractitemmodel.h"

// 组件的基类。里面的事件会由widget调用   
class Component {

protected:
    std::map<QString, QVariant> properties;
    std::vector<QString> propertyKeys;

public:
    HierarchyObject* hierarchyObject;
    QString name = "undefined";

    virtual void onInit() {};
    virtual void onUpdate() {};
    virtual void onPropertyChange(const QString& key, const QVariant& original) {};

public:
    // 获取属性的值   
    QVariant getProp(const QString& key);
    // 改变属性的值   
    void setProp(const QString& key, const QVariant& value);
    // 获取所有键   
    std::vector<QString> getPropKeys();

protected:
    // 定义属性   
    void defProp(const QString& key, const QVariant& init);



};



