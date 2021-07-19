#include "SkyboxRenderer.h"

SkyboxRenderer::SkyboxRenderer()
{
}

void SkyboxRenderer::onRender(OpenGLFunctions * gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
    if (!meshInitialized) {

        m_vertexBuffer = new QOpenGLBuffer();
        float buf[8] = { -1, -1, -1, 1, 1, -1, 1, 1 };
        m_vertexBuffer->create();
        m_vertexBuffer->bind();
        m_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_vertexBuffer->allocate(buf, 8 * sizeof(float));

        meshInitialized = true;
    }

    assert(shader != NULL);

    // 绑定shader
    shader->bind();
    m_vertexBuffer->bind();
    // offset=0 size=2 stride=default
    shader->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    // 准备shader中的矩阵 
    
    glm::mat4 projInv = glm::inverse(projection);
    glm::mat4 viewInv = glm::inverse(view);
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_PROJ_INV"), 1, GL_FALSE, glm::value_ptr(projInv));
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_VIEW_INV"), 1, GL_FALSE, glm::value_ptr(viewInv));
    
    // no depth
    gl->glDepthMask(GL_FALSE);
    
    gl->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    gl->glDepthMask(GL_TRUE);


    // 释放 
    m_vertexBuffer->release();
    shader->release();


}
