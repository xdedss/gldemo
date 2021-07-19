#pragma once
#include "Component.h"
#include "LineRenderer.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class Trail :
    public Component
{

public:
    QString name() const override { return "Trail"; };
    std::vector<glm::mat4> keypoints;
    LineRenderer* renderer = NULL;

    void onUpdate() override;

    // 检查子物体有没有更新  
    bool compareDiff();

    // 更新对应的轨迹渲染器 
    void updateRenderer();

    // 插值（得到的是local2world）    
    glm::mat4 interpolate(float t);

    // 姿态矩阵平滑插值 
    glm::mat4 slerp(glm::mat4 m1, glm::mat4 m2, float t);

};

