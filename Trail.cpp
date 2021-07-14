#include "Trail.h"
#include "HierarchyObject.h"

void Trail::onUpdate()
{
    if (compareDiff()) {
        // 如果子树有更新就更新轨迹   
        keypoints.clear();
        for (int i = 0; i < hierarchyObject->childrenCount(); i++) {
            HierarchyObject* child = hierarchyObject->getChildren(i);
            assert(child);
            keypoints.push_back(child->localToWorld());
        }
    }
}

bool Trail::compareDiff()
{
    if (keypoints.size() != hierarchyObject->childrenCount()) return true;
    for (int i = 0; i < hierarchyObject->childrenCount(); i++) {
        HierarchyObject* child = hierarchyObject->getChildren(i);
        assert(child);
        if (child->localToWorld() != keypoints[i]) return true;
    }
    return false;
}

glm::mat4 Trail::interpolate(float t)
{
    int tf = keypoints.size() - 1;
    assert(t >= 0 && t <= tf);
    // 首尾特殊处理    
    if (t <= 0.5f) {
        return slerp(keypoints[0], keypoints[1], t);
    }
    if (t >= tf - 0.5f) {
        return slerp(keypoints[tf], keypoints[tf - 1], tf - t);
    }
    // 中间贝塞尔   
    int tc = (int)round(t);
    glm::mat4 start = slerp(keypoints[tc - 1], keypoints[tc], 0.5f);
    glm::mat4 end = slerp(keypoints[tc + 1], keypoints[tc], 0.5f);
    glm::mat4 center = keypoints[tc];
    float lerpT = (t - (tc - 0.5f));
    glm::mat4 k1 = slerp(start, center, lerpT);
    glm::mat4 k2 = slerp(center, end, lerpT);
    glm::mat4 res = slerp(k1, k2, lerpT);

    return res;
}

glm::mat4 Trail::slerp(glm::mat4 m1, glm::mat4 m2, float t)
{
    assert(t >= 0 && t <= 1);
    glm::vec3 pos1 = m1 * glm::vec4(0, 0, 0, 1);
    glm::vec3 pos2 = m2 * glm::vec4(0, 0, 0, 1);
    glm::vec3 pos = pos1 * (1 - t) + pos2 * t;
    glm::quat q1 = glm::toQuat(m1);
    glm::quat q2 = glm::toQuat(m2);
    glm::quat q = glm::slerp(q1, q2, t);
    return glm::translate(glm::identity<glm::mat4>(), pos) * glm::toMat4(q);
}


