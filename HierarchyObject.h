#pragma once


#include <qpoint.h>
#include <qstring.h>
#include <qabstractitemmodel.h>

#include <vector>
#include <set>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <queue>

#include "nesteddefs.h"


/**
* @class HierarchyObject 
* @brief 表示场景树中的节点
* @note
* 继承了QAbstractItemModel，用于在treeview中显示组件的可编辑属性
*/
class HierarchyObject : public QAbstractItemModel {

private:
    /** @brief 子节点列表 */ 
    std::vector<HierarchyObject*> children;
    /** @brief 该节点挂载的组件列表 */ 
    std::vector<Component*> components;
    /** @brief 该节点的父节点 */ 
    HierarchyObject* parentObj = NULL;

public:
    /** @brief 该节点局部到该节点父节点局部坐标的变换阵 */ 
    glm::mat4 transform;
    /** @brief 节点名称 */ 
    QString name;
    /** @brief 如果为false，则其上及子节点上组件的onUpdate不会被调用 */ 
    bool enabled;

    /** @brief 子节点数量 */
    int childrenCount() { return children.size(); }
    /** @brief 获取第i个子节点 */
    HierarchyObject* getChildren(int i) { return (children.size() > i && i >= 0) ? children[i] : NULL; }
    /** @brief 寻找指定名称子节点 */
    HierarchyObject* getChildren(const QString& name);
    /** @brief 找到子节点序号 */
    int findChild(HierarchyObject* child);
    /** @brief 本节点挂载的组件数量 */
    int componentsCount() { return components.size(); }
    /** @brief 获取父节点 */
    HierarchyObject* getParent() { return parentObj; }

    // ctor
    HierarchyObject(const QString& name, HierarchyObject* parent = NULL);
    ~HierarchyObject();

    /** @brief 从局部坐标转世界坐标的矩阵 
      * @note 耗时，应缓存 */
    glm::mat4 localToWorld();
    /** @brief 从世界坐标转局部坐标的矩阵 
      * @note 耗时，应缓存 */
    glm::mat4 worldToLocal() {
        return glm::inverse(localToWorld());
    }
    
    
    /** @brief 取出子节点 
      * @note 仅调整，未通知treeview，不应被单独调用 */
    HierarchyObject* popChild(int index);
    /** @brief 放入子节点 
      * @note 仅调整，未通知treeview，不应被单独调用 */ 
    void insertChild(int index, HierarchyObject* child);
    /** @brief 调整子节点顺序 
      * @note 仅调整，未通知treeview，不应被单独调用 */ 
    void moveChild(int oldIndex, int newIndex);
    
    
    /** @brief 获取第i个组件，cast成指定类型 
      * @note 如果该组件不是指定类型，返回空指针 */ 
    template <class T>
    T* getComponent(int i) {
        if (i < 0 || i >= components.size()) return NULL;
        return dynamic_cast<T*>(components[i]);
    }

    /** @brief 获取第一个指定类型的组件 
      * @note 如果不存在该类型的组件，返回空指针 */ 
    template <class T>
    T* getComponent() {
        for (auto component : components) {
            if (dynamic_cast<T*>(component)) {
                return dynamic_cast<T*>(component);
            }
        }
        return NULL;
    }

    /** @brief 挂载组件 
      * @return 组件自身指针 */  
    Component * addComponent(Component* component);


    /** @brief 递归更新子树 */ 
    void updateRecursively();

    /** @brief 递归遍历子树 */   
    void callRecursively(const std::function<void(HierarchyObject*)>& func, bool requireEnabled = false);


    // -------------实现QAbstractItemModel的必选接口-----------------  
    // 获取给定元素的第row个子元素 
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    // 获取给定元素的父元素 
    QModelIndex parent(const QModelIndex &index) const;
    // 获取某元素的子节点数 
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    // 2
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    // 取出数据 
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // ----------------------重写可选函数----------------------------
    // 接收编辑name事件  
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    // 返回指定节点的各种显示属性  
    Qt::ItemFlags flags(const QModelIndex &index) const;
    // 返回标题  
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

};


