#pragma once


#include "nesteddefs.h"

// ����Ļ��ࡣ������¼�����widget����   
class Component {

public:
    HierarchyObject* hierarchyObject;

    virtual void onInit() {};
    virtual void onUpdate() {};

};



