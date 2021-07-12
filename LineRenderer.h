#pragma once
#include "Renderer.h"
#include "vertex.h"

class LineRenderer :
    public Renderer
{

private:
    void applyVertices();

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool modified = false;

    std::vector<Vertex> vertices;

public:
    float lineWidth = 2.0f;
    bool continuous = true;
    QVector3D highlightColor = { 1.0, 0.5, 0.0 };
    QOpenGLShaderProgram* shader = NULL;

    //ctor
    LineRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;

    void setVertices(const std::vector<Vertex>& vertices);
};

