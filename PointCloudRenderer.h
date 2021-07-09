#pragma once
#include "Renderer.h"
#include <assert.h>
#include <vector>

#include "vertex.h"


class PointCloudRenderer :
    public Renderer
{

private:

    void applyVertices();

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool modified = false;

    std::vector<Vertex> vertices;

public:
    float sizeScale = 10.0f;
    QOpenGLShaderProgram* shader = NULL;

    void onRender(glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;

    void setVertices(const std::vector<Vertex>& vertices);

};

