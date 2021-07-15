#pragma once

#include <set>

#include "nesteddefs.h"
#include "qvariant.h"
#include "qabstractitemmodel.h"

// ����Ļ��ࡣ������¼�����widget����   
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
    // ��ȡ���Ե�ֵ   
    QVariant getProp(const QString& key);
    // �ı����Ե�ֵ   
    void setProp(const QString& key, const QVariant& value);
    // ��ȡ���м�   
    std::vector<QString> getPropKeys();

protected:
    // ��������   
    void defProp(const QString& key, const QVariant& init);



};



