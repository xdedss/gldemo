#pragma once
#include "Renderer.h"
#include "vertex.h"

/**
* @class LineRenderer 
* @brief 渲染一系列线段
* @detail 其中的continuous属性代表了线段是否连续。若为true则会绘制头尾相接的线
*/
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
    /** @brief 高亮的颜色 */
    QVector3D highlightColor = { 1.0, 0.5, 0.0 };
    /** @brief 使用的着色器，默认会被设为default.vert/default.frag */
    QOpenGLShaderProgram* shader = NULL;

    //ctor
    LineRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;

    void onPropertyChange(const QString& key, const QVariant& original) override;

    void setVertices(const std::vector<Vertex>& vertices);
};

