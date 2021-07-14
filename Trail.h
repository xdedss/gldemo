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
    std::vector<glm::mat4> keypoints;
    LineRenderer* renderer = NULL;

    void onUpdate() override;

    // �����������û�и���  
    bool compareDiff();

    // ���¶�Ӧ�Ĺ켣��Ⱦ��
    void updateRenderer();

    // ��ֵ���õ�����local2world��    
    glm::mat4 interpolate(float t);

    // ��̬����ƽ����ֵ
    glm::mat4 slerp(glm::mat4 m1, glm::mat4 m2, float t);

};

