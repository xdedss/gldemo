#pragma once

#include <vector>

#include "qstring.h"
#include "qmimedata.h"
#include "qmimetype.h"

#include "nesteddefs.h"
#include "qabstractitemmodel.h"
#include "qitemselectionmodel.h"

class HierarchyModel : public QAbstractItemModel
{
    Q_OBJECT

private:
    std::vector<HierarchyObject*> objects;

public slots:
    // 选择变更
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);


public:
    HierarchyObject* root;
    Widget* widget;
    int objectCount() { return objects.size(); }
    HierarchyObject* getObject(int i) { return objects[i]; }

    HierarchyModel();
    // 创建HierarchyObject并进行一些初始化操作
    HierarchyObject* createObject(const QString& name);

    // 根据QModelIndex取出对应HierarchyObject
    HierarchyObject* index2obj(const QModelIndex &index) const;
    // 取出mime里面的拖放来源
    bool HierarchyModel::retrieveDroppedObjects(const QMimeData * data, std::vector<HierarchyObject*>& out_objects) const;


    // -------------实现QAbstractItemModel的必选接口-----------------
    // 获取给定元素的第row个子元素
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    // 获取给定元素的父元素
    QModelIndex parent(const QModelIndex &index) const;
    // 获取某元素的子节点数
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    // （永远return1）
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    // 取出数据（name）
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // ----------------------重写可选函数----------------------------
    // 接收编辑name事件
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    // 返回指定节点的各种显示属性
    Qt::ItemFlags flags(const QModelIndex &index) const;
    // 是否允许拖放到指定节点
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const; //drop是否允许
    // 拖放拖出时产生mimeData
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    // 拖放事件接收mimeData
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent); //drop事件
    // 返回允许的mimeType
    QStringList mimeTypes() const;
    // 允许的action
    Qt::DropActions supportedDropActions() const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

