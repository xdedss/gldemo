#include "LineRenderer.h"

void LineRenderer::applyVertices()
{
    //assert(shader != NULL);
    assert(vertices.size() > 0);

    if (m_vertexBuffer != NULL && m_vertexBuffer->isCreated()) {
        m_vertexBuffer->destroy();
    }
    if (m_vertexBuffer == NULL) {
        m_vertexBuffer = new QOpenGLBuffer();
    }

    m_vertexBuffer->create();
    m_vertexBuffer->bind();
    m_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer->allocate(vertices.data(), vertices.size() * sizeof(Vertex));

    //shader->enableAttributeArray(0);
    //shader->enableAttributeArray(1);

    m_vertexBuffer->release();
}

LineRenderer::LineRenderer()
{
    defProp("lineWidth", 2.0f);
    defProp("continuous", true);
}

void LineRenderer::onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
    if (modified) {
        applyVertices();
        modified = false;
    }

    if (m_vertexBuffer == NULL) return; // 没有设定顶点，不渲染 

    assert(shader != NULL);

    // 绑定shader 
    shader->bind();
    m_vertexBuffer->bind();
    shader->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    shader->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

    // 准备shader中的矩阵 
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_PROJ"), 1, GL_FALSE, glm::value_ptr(projection));
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_VIEW"), 1, GL_FALSE, glm::value_ptr(view));
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_MODEL"), 1, GL_FALSE, glm::value_ptr(model));
    
    float lineWidth = getProp("lineWidth").toFloat();

    // 画outline 
    if (highlight) {
        gl->glDepthMask(GL_FALSE);
        //gl->glUniform1f(shader->uniformLocation("sizeScale"), sizeScale * 2);
        //gl->glUniform1f(shader->uniformLocation("sizeAbsolute"), 10);
        gl->glUniform4f(shader->uniformLocation("colorOverride"), highlightColor.x(), highlightColor.y(), highlightColor.z(), 1.0);
        gl->glLineWidth(lineWidth * 2 + 5);
        gl->glDrawArrays(getProp("continuous").toBool() ? GL_LINE_STRIP : GL_LINES, 0, vertices.size());
        gl->glDepthMask(GL_TRUE);
    }

    // 正常画 
    //gl->glUniform1f(shader->uniformLocation("sizeScale"), sizeScale);
    //gl->glUniform1f(shader->uniformLocation("sizeAbsolute"), 0);
    gl->glUniform4f(shader->uniformLocation("colorOverride"), 1.0, 0.5, 0.0, 0.0);
    gl->glLineWidth(lineWidth);
    gl->glDrawArrays(getProp("continuous").toBool() ? GL_LINE_STRIP : GL_LINES, 0, vertices.size());

    gl->glLineWidth(0);

    // 释放 
    m_vertexBuffer->release();
    shader->release();

}

void LineRenderer::onPropertyChange(const QString & key, const QVariant & original)
{
}

void LineRenderer::setVertices(const std::vector<Vertex>& vertices)
{
    this->vertices = vertices;
    this->modified = true;
}
