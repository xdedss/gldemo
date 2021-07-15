#pragma once
#include "Renderer.h"
class SkyboxRenderer :
    public Renderer
{

private:

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool meshInitialized = false;

public:
    QString name() const override { return "SkyboxRenderer"; };
    QVector3D highlightColor = { 1.0, 0.5, 0.0 };
    QOpenGLShaderProgram* shader = NULL;

    //ctor
    SkyboxRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;


};

