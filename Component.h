#pragma once


#include "nesteddefs.h"

class Component {

public:
    HierarchyObject* hierarchyObject;

    virtual void onInit() {};
    virtual void onUpdate() {};

};



