#pragma once

#include <vector>
#include <map>
#include <qapplication.h>
#include <qpoint.h>

static class Input {

private:

    static std::map<Qt::MouseButton, bool> mouseButtonState, mouseButtonStateCache, mouseButtonStatePrevCache;
    static std::map<Qt::Key, bool> keyState, keyStateCache, keyStatePrevCache;
    static float mouseDelta, mouseDeltaCache;
    static std::map<QString, float> axisValues, axisValuesCache, axisValuesPrevCache;

    template <class TKey, class TValue>
    static TValue mapDefaultGet(std::map<TKey, TValue>& m, TKey key, TValue defaultValue) {
        if (m.count(key) == 0) return defaultValue;
        return m[key];
    }

public:

    // ---key----
    static bool getKey(Qt::Key keycode) {
        return mapDefaultGet(keyStateCache, keycode, false);
    }
    static bool getKeyDown(Qt::Key keycode) {
        return mapDefaultGet(keyStateCache, keycode, false) && !mapDefaultGet(keyStatePrevCache, keycode, false);
    }
    static bool getKeyUp(Qt::Key keycode) {
        return (!mapDefaultGet(keyStateCache, keycode, false)) && mapDefaultGet(keyStatePrevCache, keycode, false);
    }
    static void setKey(Qt::Key keycode, bool down) {
        keyState[keycode] = down;
    }

    // ---mouse---
    static bool getMouseButton(Qt::MouseButton i) {
        return mapDefaultGet(mouseButtonStateCache, i, false);
    }
    static bool getMouseButtonDown(Qt::MouseButton i) {
        return mapDefaultGet(mouseButtonStateCache, i, false) && !mapDefaultGet(mouseButtonStatePrevCache, i, false);
    }
    static bool getMouseButtonUp(Qt::MouseButton i) {
        return (!mapDefaultGet(mouseButtonStateCache, i, false)) && mapDefaultGet(mouseButtonStatePrevCache, i, false);
    }
    static void setMouseButton(Qt::MouseButton i, bool down) {
        //assert(i >= 0 && i < 3);
        mouseButtonState[i] = down;
    }

    // ----mouse wheel----
    static float getMouseWheelDelta() {
        return mouseDeltaCache;
    }
    static void setMouseWheelDelta(float delta) {
        mouseDelta = delta;
    }

    // ----axis-----
    static float getAxis(const QString& name) {
        return mapDefaultGet(axisValuesCache, name, 0.0f);
    }
    static float getAxisDelta(const QString& name) {
        return mapDefaultGet(axisValuesCache, name, 0.0f) - mapDefaultGet(axisValuesPrevCache, name, 0.0f);
    }
    static void setAxis(const QString& name, float value) {
        axisValues[name] = value;
    }


    static void beforeUpdate() {
        // copy
        keyStatePrevCache = keyStateCache;
        mouseButtonStatePrevCache = mouseButtonStateCache;
        axisValuesPrevCache = axisValuesCache;
        keyStateCache = keyState;
        mouseButtonStateCache = mouseButtonState;
        axisValuesCache = axisValues;
        // mouse wheel
        mouseDeltaCache = mouseDelta;
        mouseDelta = 0; // clear
    }


};

