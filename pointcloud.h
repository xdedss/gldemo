#pragma once

#ifndef POINTCLOUD
#define POINTCLOUD

#include <qopenglfunctions_4_3_core.h>
#include <qopenglshaderprogram.h>
#include <qopenglvertexarrayobject.h>
#include <qopenglbuffer.h>
#include <qpoint.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertex.h"
#include "nanoflann.hpp"

// 弃用的（0710），应使用PointCloudRenderer
class PointCloud : protected QOpenGLFunctions_4_3_Core {

private:
    //QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool modified = false;
    bool initialized = false;
    std::vector<Vertex> vertices;

    void applyVertices() {

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

public:
    glm::mat4 transform;
    QOpenGLShaderProgram* shader = NULL;

    PointCloud(QOpenGLShaderProgram* shader = NULL) {
        transform = glm::identity<glm::mat4>();
        vertices.clear();
        this->shader = shader;
    }

    //~PointCloud() {
    //    delete m_vertexBuffer;
    //    delete index;
    //}

    void setVertices(const std::vector<Vertex>& vertices) {
        this->vertices = vertices;
        this->modified = true;
    }


    void render(glm::mat4 projection, glm::mat4 view) {
        if (!initialized) {
            initializeOpenGLFunctions();
            initialized = true;
        }
        if (modified) {
            applyVertices();
            modified = false;
        }
        bind();
        glUniformMatrix4fv(shader->uniformLocation("MAT_PROJ"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(shader->uniformLocation("MAT_VIEW"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shader->uniformLocation("MAT_MODEL"), 1, GL_FALSE, glm::value_ptr(transform));
        glDrawArrays(GL_POINTS, 0, vertices.size());
        release();
    }

    void bind() {
        assert(shader != NULL);
        shader->bind();
        m_vertexBuffer->bind();
        shader->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
        shader->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
    }

    void release() {
        m_vertexBuffer->release();
        shader->release();
    }


    // ---------nanoflann kdtree interface-----------
    // Must return the number of data poins
    inline size_t kdtree_get_point_count() const { return vertices.size(); }
    // Must return the dim'th component of the idx'th point in the class:
    inline float kdtree_get_pt(const size_t idx, int dim) const { 
        if (dim == 0) return vertices[idx].position().x(); 
        if (dim == 1) return vertices[idx].position().y(); 
        if (dim == 2) return vertices[idx].position().z(); 
    }
    template <class BBOX>
    bool kdtree_get_bbox(BBOX &bb) const { return false; }


    //// kd search
    //QVector3D nearestSearch(QVector3D startPoint) {
    //    size_t ret_index;
    //    float out_dist_sqr;
    //    nanoflann::KNNResultSet<float> resultSet(1);
    //    resultSet.init(&ret_index, &out_dist_sqr);
    //    float inputPoint[3]{ startPoint.x(), startPoint.y(), startPoint.z() };
    //    index->findNeighbors(resultSet, inputPoint, nanoflann::SearchParams(10));
    //    qDebug() << "ret_index " << ret_index << " dist: " << out_dist_sqr << " | " << vertices[ret_index].position();
    //    return vertices[ret_index].position();
    //}

};









#endif // !POINTCLOUD


