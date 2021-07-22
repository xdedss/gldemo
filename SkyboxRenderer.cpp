#include "SkyboxRenderer.h"

SkyboxRenderer::SkyboxRenderer()
{
}

void SkyboxRenderer::onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model)
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

    float temptime;
    /*   glm::vec3 skycolorchange = { 0.0, 0.4, 0.7 };
       glm::vec3 groundcolorchange= { 0.5, 0.5, 0.5 };*/
       //日出颜色 { 0,0.22,0.58 } { 0.4, 0.4, 0.4 } { 0.3,0.4,0.9 } { 0.8, 0.9, 1.0 }
       //早上颜色 { 0.0, 0.4, 0.7 } { 0.5, 0.5, 0.5 } { 0.5, 0.5, 0.9 } { 0.8, 1.0, 1.0 } 
       //中午颜色 {0.0，0.37,1.0} {0.6,0.6,0.6}  { 0.5, 0.5, 0.9 } { 0.8, 1.0, 1.0 } 
       //傍晚颜色 {}  {}  {}  {}
       //夜晚颜色 {0,0.0,0.13} {0.2,0.2,0.2} {0,0.0,0.25}  { 0.0, 0.0, 0.3}  
    glm::vec3 skycolorchange = { 0,0.0,0.13 };
    glm::vec3 groundcolorchange = { 0.2,0.2,0.2 };
    glm::vec3 sladderchange = { 0,0.0,0.25 };
    glm::vec3 gladderchange = { 0.0, 0.0, 0.3 };


    temptime = timetime / 10.0;
    //夜晚-黎明  
    if (temptime <= 6) {
        skycolorchange = glm::vec3(0, 0.0, 0.0) + glm::vec3(0, temptime * 0.0092, temptime * 0.0188);
        sladderchange = glm::vec3(0.0, 0.0, 0.25) + glm::vec3(temptime * 0.0125, temptime * 0.0167, temptime * 0.0271);
        gladderchange = glm::vec3(0.0, 0.0, 0.3) + glm::vec3(temptime * 0.0443, temptime * 0.05, temptime * 0.0390);
        groundcolorchange = glm::vec3(0.2, 0.2, 0.2) + glm::vec3(temptime * 0.0083, temptime * 0.0083, temptime * 0.0083);
    }
    //日出 
    else if (temptime <= 8) {
        skycolorchange = glm::vec3(0, 0.0552, 0.1128) + glm::vec3(0, (temptime - 6.0) * 0.0824, (temptime - 6.0) * 0.2336);
        sladderchange = glm::vec3(0.0750, 0.1002, 0.4126) + glm::vec3((temptime - 6.0) * 0.1125, (temptime - 6.0) * 0.1499, (temptime - 6.0) * 0.2437);
        gladderchange = glm::vec3(0.2658, 0.3, 0.534) + glm::vec3((temptime - 6.0) * 0.2671, (temptime - 6.0) * 0.3, (temptime - 6.0) * 0.233);
        groundcolorchange = glm::vec3(0.2498, 0.2498, 0.2498) + glm::vec3((temptime - 6.0) * 0.0751, (temptime - 6.0) * 0.0751, (temptime - 6.0) * 0.0751);
    }

    //天亮 
    else if (temptime <= 10) {
        skycolorchange = glm::vec3(0, 0.22, 0.58) + glm::vec3(0.0, (temptime - 8.0) * 0.09, (temptime - 8.0) * 0.06);
        groundcolorchange = glm::vec3(0.4, 0.4, 0.4) + glm::vec3((temptime - 8.0) * 0.05, (temptime - 8.0) * 0.05, (temptime - 8.0) * 0.05);
        sladderchange = glm::vec3(0.3, 0.4, 0.9) + glm::vec3((temptime - 8.0) * 0.1, (temptime - 8.0) * 0.05, 0.0);
        gladderchange = glm::vec3(0.8, 0.9, 1.0) + glm::vec3(0.0, (temptime - 8.0) * 0.05, 0.0);
    }
    //白天 
    else if (temptime <= 16) {
        skycolorchange = glm::vec3(0.0, 0.4, 0.7);
        groundcolorchange = glm::vec3(0.5, 0.5, 0.5);
        sladderchange = glm::vec3(0.5, 0.5, 0.9);
        gladderchange = glm::vec3(0.8, 1.0, 1.0);

    }

    //日落 
    else if (temptime <= 18) {

        skycolorchange = glm::vec3(0.0, 0.4, 0.7) - glm::vec3(0.0, (temptime - 16.0) * 0.09, (temptime - 16.0) * 0.06);
        groundcolorchange = glm::vec3(0.5, 0.5, 0.5) - glm::vec3((temptime - 16.0) * 0.05, (temptime - 16.0) * 0.05, (temptime - 16.0) * 0.05);
        sladderchange = glm::vec3(0.5, 0.5, 0.9) - glm::vec3((temptime - 16.0) * 0.1, (temptime - 16.0) * 0.05, 0.0);
        gladderchange = glm::vec3(0.8, 1.0, 1.0) - glm::vec3((temptime - 16.0) * 0.05, (temptime - 16.0) * 0.2, (temptime - 16.0) * 0.2);
    }

    //天黑 
    else if (temptime <= 20) {

        skycolorchange = glm::vec3(0, 0.22, 0.58) - glm::vec3(0, (temptime - 18.0) * 0.0824, (temptime - 18.0) * 0.2336);
        groundcolorchange = glm::vec3(0.4, 0.4, 0.4) - glm::vec3((temptime - 18.0) * 0.0751, (temptime - 18.0) * 0.0751, (temptime - 18.0) * 0.0751);
        sladderchange = glm::vec3(0.3, 0.4, 0.9) - glm::vec3((temptime - 18.0) * 0.1125, (temptime - 18.0) * 0.1499, (temptime - 18.0) * 0.2437);
        gladderchange = glm::vec3(0.7, 0.6, 0.6) - glm::vec3((temptime - 18.0) * 0.2251, (temptime - 18.0) * 0.15, (temptime - 18.0) * 0.033);

    }

    //夜晚
    else {
        skycolorchange = glm::vec3(0, 0.0552, 0.1128) - glm::vec3(0, (temptime - 20.0) * 0276 / 2, (temptime - 20.0) * 0.0564 / 2);
        sladderchange = glm::vec3(0.0750, 0.1002, 0.4126) - glm::vec3((temptime - 20.0) * 0.0375 / 2, (temptime - 20.0) * 0.0501 / 2, (temptime - 20.0) * 0.0813 / 2);
        gladderchange = glm::vec3(0.2658, 0.3, 0.534) - glm::vec3((temptime - 20.0) * 0.1342 / 2, (temptime - 20.0) * 0.15 / 2, (temptime - 20.0) * 0.117 / 2);
        groundcolorchange = glm::vec3(0.2498, 0.2498, 0.2498) - glm::vec3((temptime - 20.0) * 0.0299 / 2, (temptime - 20.0) * 0.0299 / 2, (temptime - 20.0) * 0.0299 / 2);
    }


    glm::mat4 projInv = glm::inverse(projection);
    glm::mat4 viewInv = glm::inverse(view);
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_PROJ_INV"), 1, GL_FALSE, glm::value_ptr(projInv));
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_VIEW_INV"), 1, GL_FALSE, glm::value_ptr(viewInv));
    gl->glUniform3f(shader->uniformLocation("skyColor_change"), skycolorchange.x, skycolorchange.y, skycolorchange.z);
    gl->glUniform3f(shader->uniformLocation("groundColor_change"), groundcolorchange.x, groundcolorchange.y, groundcolorchange.z);
    gl->glUniform3f(shader->uniformLocation("ladder1"), gladderchange.x, gladderchange.y, gladderchange.z);
    gl->glUniform3f(shader->uniformLocation("ladder2"), sladderchange.x, sladderchange.y, sladderchange.z);
    // no depth
    gl->glDepthMask(GL_FALSE);

    gl->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    gl->glDepthMask(GL_TRUE);


    // 释放 
    m_vertexBuffer->release();
    shader->release();


}
