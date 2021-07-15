#include "Component.h"



static std::map<QString, QVariant> defaultProp = {
    {"enabled", true},
};

QVariant Component::getProp(const QString & key)
{
    if (properties.count(key) > 0) {
        return properties[key];
    }
    if (defaultProp.count(key) > 0) {
        return defaultProp[key];
    }
    throw "no such property!";
}

void Component::setProp(const QString & key, const QVariant& value)
{
    QVariant original = getProp(key);
    if (original != value) {
        properties[key] = value;
        onPropertyChange(key, original);
    }
}

std::vector<QString> Component::getPropKeys()
{
    std::vector<QString> res;
    for (auto& pair : defaultProp) {
        res.push_back(pair.first);
    }
    res.insert(res.end(), propertyKeys.begin(), propertyKeys.end());
    return res;
}

void Component::defProp(const QString & key, const QVariant & init)
{
    assert(properties.count(key) == 0);
    assert(defaultProp.count(key) == 0);
    properties[key] = init;
    propertyKeys.push_back(key);
}
