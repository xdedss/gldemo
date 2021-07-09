#include "PointCloudRenderer.h"
#include "Component.h"
#include "HierarchyObject.h"
#include "widget.h"

void PointCloudRenderer::applyVertices()
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

void PointCloudRenderer::onRender(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
    auto gl = hierarchyObject->widget->functions();

    if (modified) {
        applyVertices();
        modified = false;
    }

    //assert(shader != NULL);
    if (shader == NULL) {
        shader = hierarchyObject->widget->shaders["default"];
    }


    shader->bind();
    m_vertexBuffer->bind();
    shader->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    shader->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
    
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_PROJ"), 1, GL_FALSE, glm::value_ptr(projection));
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_VIEW"), 1, GL_FALSE, glm::value_ptr(view));
    gl->glUniformMatrix4fv(shader->uniformLocation("MAT_MODEL"), 1, GL_FALSE, glm::value_ptr(model));

    // 画outline
    if (highlight) {
        gl->glDepthMask(GL_FALSE);
        gl->glUniform1f(shader->uniformLocation("sizeScale"), sizeScale * 2);
        gl->glUniform1f(shader->uniformLocation("sizeAbsolute"), 10);
        gl->glUniform4f(shader->uniformLocation("colorOverride"), 1.0, 0.5, 0.0, 1.0);
        gl->glDrawArrays(GL_POINTS, 0, vertices.size());
        gl->glDepthMask(GL_TRUE);
    }

    // 正常画
    gl->glUniform1f(shader->uniformLocation("sizeScale"), sizeScale);
    gl->glUniform1f(shader->uniformLocation("sizeAbsolute"), 0);
    gl->glUniform4f(shader->uniformLocation("colorOverride"), 1.0, 0.5, 0.0, 0.0);
    gl->glDrawArrays(GL_POINTS, 0, vertices.size());

    m_vertexBuffer->release();
    shader->release();
    

}

void PointCloudRenderer::setVertices(const std::vector<Vertex>& vertices)
{
    this->vertices = vertices;
    this->modified = true;
}
