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
        updateRenderer();
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

void Trail::updateRenderer()
{
    // 从hierarchyObject上获取LineRenderer 
    if (renderer == NULL) {
        renderer = hierarchyObject->getComponent<LineRenderer>();
    }
    // 如果没有的话就添加一个   
    if (renderer == NULL) {
        renderer = new LineRenderer();
        renderer->setProp("lineWidth", 1.0f);
        renderer->setProp("continuous", false);
        hierarchyObject->addComponent(renderer);
    }

    const int numSegments = 10; // 每两个关键点之间的渲染段数  

    QVector3D color = { 0.0, 1.0, 0.0 };
    std::vector<Vertex> vertices;
    if (keypoints.size() > 0) {
        glm::vec3 pos0 = interpolate(0) * glm::vec4(0, 0, 0, 1);
        vertices.push_back(Vertex({ pos0.x, pos0.y, pos0.z }, color));
    }
    for (int i = 0; i < keypoints.size() - 1; i++) {
        for (int j = 1; j <= numSegments; j++) {
            glm::vec3 pos = interpolate(i + (float)j / numSegments) * glm::vec4(0, 0, 0, 1);
            vertices.push_back(Vertex({ pos.x, pos.y, pos.z }, color));
        }
    }
    renderer->setVertices(vertices);

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
    glm::vec3 pos1, s1, sk1;
    glm::vec3 pos2, s2, sk2;
    glm::vec4 p1, p2;
    glm::quat q1;
    glm::quat q2;
    glm::decompose(m1, s1, q1, pos1, sk1, p1);
    glm::decompose(m2, s2, q2, pos2, sk2, p2);
    glm::vec3 pos = pos1 * (1 - t) + pos2 * t;
    glm::quat q = glm::slerp(q1, q2, t);
    return glm::translate(glm::identity<glm::mat4>(), pos) * glm::toMat4(q);
}


