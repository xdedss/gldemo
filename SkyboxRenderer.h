#pragma once
#include "Renderer.h"

/**
* @class SkyboxRenderer
* @brief 继承自Renderer，用于渲染天空
*/
class SkyboxRenderer :
    public Renderer
{

private:

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool meshInitialized = false;

public:
    /** @brief "SkyboxRenderer" */
    QString name() const override { return "SkyboxRenderer"; };
    QVector3D highlightColor = { 1.0, 0.5, 0.0 };
    /** @brief 渲染所用shader，默认会被设为skybox.vert/skybox.frag */
    QOpenGLShaderProgram* shader = NULL;
    int timetime = 0;
    //ctor
    SkyboxRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;


};

