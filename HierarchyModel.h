#pragma once

#include <vector>

#include "qstring.h"

#include "nesteddefs.h"
#include "qabstractitemmodel.h"

class HierarchyModel : public QAbstractItemModel
{

private:
    std::vector<HierarchyObject*> objects;

public:
    Widget* widget;
    int objectCount() { return objects.size(); }
    HierarchyObject* getObject(int i) { return objects[i]; }
    HierarchyObject* createObject(const QString& name);
    std::vector<HierarchyObject*> rootObjects();

    // QAbstractItemModel
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

