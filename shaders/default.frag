
#version 330 core

out vec4 FragColor;  
in vec3 vertColor;


void main()
{
	FragColor = vec4(vertColor, 1.0);
}

