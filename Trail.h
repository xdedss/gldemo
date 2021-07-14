#pragma once
#include "Component.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class Trail :
    public Component
{

public:
    std::vector<glm::mat4> keypoints;

    void onUpdate() override;

    // 检查子物体有没有更新  
    bool compareDiff();

    // 插值（得到的是local2world）    
    glm::mat4 interpolate(float t);

    // 姿态矩阵平滑插值
    glm::mat4 slerp(glm::mat4 m1, glm::mat4 m2, float t);

};

