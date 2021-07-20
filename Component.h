#pragma once

#include <set>

#include "nesteddefs.h"
#include "qvariant.h"
#include "qabstractitemmodel.h"

/**
* @class Component 
* @brief 组件的基类。里面的事件会由widget调用
* @note
* 继承Component类的组件可以被挂载到HierarchyObject上，由Widget调度
*/
class Component {

protected:
    
    /** @brief 所有非默认参数的键值对 */
    std::map<QString, QVariant> properties;
    /** @brief 所有非默认参数的键 */
    std::vector<QString> propertyKeys;

public:
    /** @brief 指向这个组件挂载的对象 */
    HierarchyObject* hierarchyObject;
    /** @brief 该类型的名称，应由子类重写 */
    virtual QString name() const { return "undefined"; };

    virtual void onInit() {};
    /** @brief 每帧被调用一次 */
    virtual void onUpdate() {};
    /** @brief 有属性被更改时被调用一次 */
    virtual void onPropertyChange(const QString& key, const QVariant& original) {};

public:
    /** @brief 获取属性的值 */
    QVariant getProp(const QString& key);
    /** @brief 改变属性的值 */
    void setProp(const QString& key, const QVariant& value);
    /** @brief 获取所有键 */
    std::vector<QString> getPropKeys();

protected:
    /** @brief 定义属性，应当只在子类的构造函数中使用 */
    void defProp(const QString& key, const QVariant& init);



};



