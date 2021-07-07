

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertColor;

uniform mat4 MAT_MODEL;
uniform mat4 MAT_VIEW;
uniform mat4 MAT_PROJ;

void main()
{
	gl_Position = MAT_PROJ * MAT_VIEW * MAT_MODEL * vec4(aPos, 1.0);
	vertColor = aColor;
}