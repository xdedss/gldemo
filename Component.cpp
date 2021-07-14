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

void Component::setProp(const QString & key, QVariant value)
{
    properties[key] = value;
}
