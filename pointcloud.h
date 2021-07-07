#pragma once

#ifndef POINTCLOUD
#define POINTCLOUD

#include <qopenglfunctions_4_3_core.h>
#include <qopenglshaderprogram.h>
#include <qopenglvertexarrayobject.h>
#include <qopenglbuffer.h>

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertex.h"

class PointCloud : protected QOpenGLFunctions_4_3_Core {

private:
    //QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer* m_vertexBuffer = NULL;

public:
    glm::mat4 transform;
    std::vector<Vertex> vertices;
    QOpenGLShaderProgram* shader = NULL;

    PointCloud(QOpenGLShaderProgram* shader = NULL) {
        transform = glm::identity<glm::mat4>();
        vertices.clear();
        this->shader = shader;
    }

    void init() {

        initializeOpenGLFunctions();

    }

    void applyVertices() {

        assert(shader != NULL);
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

        shader->enableAttributeArray(0);
        shader->enableAttributeArray(1);

        m_vertexBuffer->release();
    }

    void render() {
        glUniformMatrix4fv(shader->uniformLocation("MAT_MODEL"), 1, GL_FALSE, glm::value_ptr(transform));
        bind();
        glDrawArrays(GL_POINTS, 0, vertices.size());
        release();
    }

    void bind() {
        //m_vao.bind();
        m_vertexBuffer->bind();
        shader->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
        shader->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());
    }

    void release() {
        m_vertexBuffer->release();
    }

};









#endif // !POINTCLOUD


