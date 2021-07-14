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
    virtual void onPropertyChange(const QString& key, const QVariant& original) {};

public:
    // ��ȡ���Ե�ֵ   
    QVariant getProp(const QString& key);
    // �ı����Ե�ֵ   
    void setProp(const QString& key, const QVariant& value);

protected:
    // ��������   
    void defProp(const QString& key, const QVariant& init);


public:
    QVariant getProp(const QString& key);
    void setProp(const QString& key, QVariant value);


};



