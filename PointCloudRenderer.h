#pragma once
#include "Renderer.h"
#include <assert.h>
#include <vector>

#include "vertex.h"
#include "nanoflann.hpp"

// kd树定义
class VerticesAdaptor {
public:
    std::vector<Vertex> vertices;
    VerticesAdaptor() {
    }
    // Must return the number of data poins
    inline size_t kdtree_get_point_count() const { 
        size_t res = vertices.size();
        return res;
    }
    // Must return the dim'th component of the idx'th point in the class:
    inline float kdtree_get_pt(const size_t idx, int dim) const { 
        if (dim == 0) return vertices[idx].position().x();
        if (dim == 1) return vertices[idx].position().y();
        if (dim == 2) return vertices[idx].position().z();
        return 0;
    }

    template <class BBOX>
    bool kdtree_get_bbox(BBOX &bb) const
    {
        return false;
    }

};

typedef nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<float, VerticesAdaptor>,
    VerticesAdaptor,
    3 /* dim */
> kd_tree_t;




class PointCloudRenderer :
    public Renderer
{

private:

    void applyVertices();

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool modified = false;

    std::vector<Vertex> vertices;

    kd_tree_t* kdtree;
    VerticesAdaptor adaptor;

public:
    float sizeScale = 10.0f;
    QVector3D highlightColor = { 1.0, 0.5, 0.0 };
    QOpenGLShaderProgram* shader = NULL;

    //ctor
    PointCloudRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;

    void setVertices(const std::vector<Vertex>& vertices);

    int vertexCount();
    Vertex getVertex(int i);

    size_t nearestSearch(QVector3D pos);

};

