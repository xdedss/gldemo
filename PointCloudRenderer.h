#pragma once
#include "Renderer.h"
#include <assert.h>
#include <vector>

#include "vertex.h"
#include "nanoflann.hpp"

// nanoflann的kd树接口实现   
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
    // 不计算bbox，返回false  
    template <class BBOX>
    bool kdtree_get_bbox(BBOX &bb) const
    {
        return false;
    }

};

// nanoflann查询用模板定义  
typedef nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<float, VerticesAdaptor>,
    VerticesAdaptor,
    3 /* dim */
> kd_tree_t;



//  渲染点云   
class PointCloudRenderer :
    public Renderer
{

private:

    // 更新顶点，放入显存    
    void applyVertices();

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool modified = false;

    std::vector<Vertex> vertices;

    kd_tree_t* kdtree;
    VerticesAdaptor adaptor;


public:
    QString name() const override { return "PointCloudRenderer"; };
    // 随距离变化的缩放系数  
    //float sizeScale = 10.0f;
    // 高亮颜色 默认橙色   
    //QVector3D highlightColor = { 1.0, 0.5, 0.0 };
    QOpenGLShaderProgram* shader = NULL;

    //ctor
    PointCloudRenderer();
    ~PointCloudRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;

    void setVertices(const std::vector<Vertex>& vertices);

    int vertexCount();
    Vertex getVertex(int i);

    // 最邻近搜索点 
    size_t nearestSearch(QVector3D pos);
    // N最邻近 
    std::vector<size_t> nearestSearch(QVector3D pos, int k);

    std::vector<Vertex> getVertices();
};

