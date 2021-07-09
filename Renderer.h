#pragma once

#include <qpoint.h>

#include <vector>
#include <set>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <qopenglfunctions_4_3_core.h>
#include <qopenglshaderprogram.h>
#include <qopenglvertexarrayobject.h>
#include <qopenglbuffer.h>

#include "Component.h"

class Renderer : public Component {

private:


public:
    virtual void onRender(glm::mat4 projection, glm::mat4 view, glm::mat4 model) = 0;

    void onInit() override;
    void onUpdate() override;

};


