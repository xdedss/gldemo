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

PointCloudRenderer::PointCloudRenderer()
{
    kdtree = new kd_tree_t(3, adaptor, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    qDebug() << ((intptr_t)&kdtree->dataset.vertices);
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

    // 绑定shader
    shader->bind();
    m_vertexBuffer->bind();
    shader->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
    shader->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

    // 准备shader中的矩阵
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

    // 释放
    m_vertexBuffer->release();
    shader->release();
    

}

void PointCloudRenderer::setVertices(const std::vector<Vertex>& vertices)
{
    this->vertices = vertices;
    this->modified = true;

    //qDebug() << ((intptr_t)&kdtree->dataset.vertices);
    adaptor.vertices = vertices;
    kdtree->buildIndex();
}

int PointCloudRenderer::vertexCount()
{
    return vertices.size();
}

Vertex PointCloudRenderer::getVertex(int i)
{
    return vertices[i];
}

size_t PointCloudRenderer::nearestSearch(QVector3D pos)
{
    size_t ret_index;
    float out_dist_sqr;
    nanoflann::KNNResultSet<float> resultSet(1);
    resultSet.init(&ret_index, &out_dist_sqr);
    float queryPoints[3] = { pos.x(), pos.y(), pos.z() };
    kdtree->findNeighbors(resultSet, &queryPoints[0], nanoflann::SearchParams(10));
    return ret_index;
}

std::vector<Vertex> PointCloudRenderer::getVertices()
{
    return vertices;
}
