#pragma once


#include "nesteddefs.h"
#include "qvariant.h"
#include "qabstractitemmodel.h"

// ����Ļ��ࡣ������¼�����widget����   
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



