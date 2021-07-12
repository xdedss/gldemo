

#version 330 core

layout (location = 0) in vec2 xy;

out vec3 worldDir;

uniform mat4 MAT_PROJ_INV;
uniform mat4 MAT_VIEW_INV;

void main()
{
    gl_Position = vec4(xy.x, xy.y, 0.999, 1.0); // @far clip plane
    // compute world direction
    vec3 camPosDir = (MAT_PROJ_INV * vec4(xy.x, xy.y, 0.0, 1.0)).xyz;
    worldDir = (MAT_VIEW_INV * vec4(camPosDir, 0.0)).xyz;
}
