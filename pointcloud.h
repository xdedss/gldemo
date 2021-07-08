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

};









#endif // !POINTCLOUD


