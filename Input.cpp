#include "Input.h"

//static std::map<Qt::MouseButton, bool> mouseButtonState, mouseButtonStateCache, mouseButtonStatePrevCache;
//static std::map<Qt::Key, bool> keyState, keyStateCache, keyStatePrevCache;

// 静态变量需要初始化

std::map<Qt::MouseButton, bool> Input::mouseButtonState = {}, Input::mouseButtonStateCache = {}, Input::mouseButtonStatePrevCache = {};
std::map<Qt::Key, bool> Input::keyState = {}, Input::keyStateCache = {}, Input::keyStatePrevCache = {};
std::map<QString, float> Input::axisValues = {}, Input::axisValuesCache = {}, Input::axisValuesPrevCache = {};
float Input::mouseDelta = 0, Input::mouseDeltaCache = 0;
