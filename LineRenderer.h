#pragma once
#include "Renderer.h"
#include "vertex.h"

// 渲染一系列直线  
class LineRenderer :
    public Renderer
{

private:
    void applyVertices();

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool modified = false;

    std::vector<Vertex> vertices;

public:
    QString name() const override { return "LineRenderer"; };
    // 线宽  
    //float lineWidth = 2.0f;
    // 是否连续  
    //bool continuous = true;
    // 高亮的颜色  
    QVector3D highlightColor = { 1.0, 0.5, 0.0 };
    QOpenGLShaderProgram* shader = NULL;

    //ctor
    LineRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;

    void onPropertyChange(const QString& key, const QVariant& original) override;

    void setVertices(const std::vector<Vertex>& vertices);
};

