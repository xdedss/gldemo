#pragma once
#include "Renderer.h"
#include <assert.h>
#include <vector>

#include "vertex.h"
#include "nanoflann.hpp"


/**
* @class VerticesAdaptor 
* @brief nanoflann的kd树接口实现
*/
class VerticesAdaptor {
public:
    /** @brief 存储要访问的顶点数据 */
    std::vector<Vertex> vertices;
    VerticesAdaptor() {
    }
    // Must return the number of data poins
    /** @brief 返回顶点数量 */
    inline size_t kdtree_get_point_count() const { 
        size_t res = vertices.size();
        return res;
    }
    // Must return the dim'th component of the idx'th point in the class:
    /** @brief 返回顶点数据 */
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
/**
* @class PointCloudRenderer 
* @brief 点云渲染器
*/
class PointCloudRenderer :
    public Renderer
{

private:

    /** @brief 将当前数据放入显存 */
    void applyVertices();

    QOpenGLBuffer* m_vertexBuffer = NULL;
    bool modified = false;

    std::vector<Vertex> vertices;

    /** @brief 指向kdtree索引的指针 */
    kd_tree_t* kdtree;
    VerticesAdaptor adaptor;


public:
    /** @brief "PointCloudRenderer" */
    QString name() const override { return "PointCloudRenderer"; };
    /** @brief 使用的着色器，默认为default.vert/default.frag */
    QOpenGLShaderProgram* shader = NULL;

    //ctor
    PointCloudRenderer();
    ~PointCloudRenderer();

    void onRender(OpenGLFunctions* gl, glm::mat4 projection, glm::mat4 view, glm::mat4 model) override;

    /** @brief 改变顶点数据 */
    void setVertices(const std::vector<Vertex>& vertices);

    /** @brief 顶点数量 */
    int vertexCount();
    /** @brief 返回第i个顶点数据 */
    Vertex getVertex(int i);

    /** @brief 搜索，返回最邻近搜索点的索引 */
    size_t nearestSearch(QVector3D pos);
    
    /** @brief 搜索，返回前k个邻近点的索引 */
    std::vector<size_t> nearestSearch(QVector3D pos, int k);

    /** @brief 获取当前顶点数据 */
    std::vector<Vertex> getVertices();
};

