#include "Input.h"

//static std::map<Qt::MouseButton, bool> mouseButtonState, mouseButtonStateCache, mouseButtonStatePrevCache;
//static std::map<Qt::Key, bool> keyState, keyStateCache, keyStatePrevCache;

std::map<Qt::MouseButton, bool> Input::mouseButtonState = {}, Input::mouseButtonStateCache = {}, Input::mouseButtonStatePrevCache = {};
std::map<Qt::Key, bool> Input::keyState = {}, Input::keyStateCache = {}, Input::keyStatePrevCache = {};
float Input::mouseDelta = 0, Input::mouseDeltaCache = 0;
