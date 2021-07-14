#pragma once

#include <qpoint.h>

#include <vector>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <qopenglshaderprogram.h>
#include <qopenglvertexarrayobject.h>
#include <qopenglbuffer.h>

#include "Component.h"
#include "commondefs.h"

class Renderer : public Component {

private:


public:
    bool highlight = false;
    // 渲染事件，每帧会被调用   
    virtual void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) = 0;

    virtual void onInit() override;
    virtual void onUpdate() override;

};


