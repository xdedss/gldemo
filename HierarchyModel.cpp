#include "HierarchyModel.h"
#include "HierarchyObject.h"
#include "widget.h"

HierarchyObject * HierarchyModel::createObject(const QString& name)
{
    HierarchyObject * res = new HierarchyObject(name);
    res->widget = widget;
    objects.push_back(res);
    return res;
}

std::vector<HierarchyObject*> HierarchyModel::rootObjects()
{
    std::vector<HierarchyObject*> res;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]->getParent() == NULL) {
            res.push_back(objects[i]);
        }
    }
    return res;
}

QModelIndex HierarchyModel::index(int row, int column, const QModelIndex & parent) const
{
    parent.
    return QModelIndex();
}

QModelIndex HierarchyModel::parent(const QModelIndex & index) const
{
    return QModelIndex();
}

int HierarchyModel::rowCount(const QModelIndex & parent) const
{
    return 0;
}

int HierarchyModel::columnCount(const QModelIndex & parent) const
{
    return 0;
}

QVariant HierarchyModel::data(const QModelIndex & index, int role) const
{
    return QVariant();
}

