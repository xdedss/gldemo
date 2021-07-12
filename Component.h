#pragma once


#include "nesteddefs.h"

// 组件的基类。里面的事件会由widget调用   
class Component {

public:
    HierarchyObject* hierarchyObject;

    virtual void onInit() {};
    virtual void onUpdate() {};

};



