
#version 330 core

out vec4 FragColor;  
in vec3 worldDir;

#define pi 3.1415926535
#define CONDITION_LERP(cond,res,t1,t2,c1,c2) \
(cond >= t1 && cond <= t2) {\
    res = ((c2) - (c1)) * ((cond) - (t1)) / ((t2) - (t1)) + (c1);\
}


void main()
{
	
	float elevation = atan(worldDir.y / length(worldDir.xz));
	float azimuth = atan(-worldDir.z, worldDir.x);
	
	vec3 skyColor = vec3(1.0, 0.0, 1.0);
	if CONDITION_LERP(elevation, skyColor, -pi/2, -0.1, vec3(0.5, 0.5, 0.5), vec3(0.5, 0.5, 0.5))
	else if CONDITION_LERP(elevation, skyColor, -0.1, 0.0, vec3(0.5, 0.5, 0.5), vec3(0.8, 1.0, 1.0))
	else if CONDITION_LERP(elevation, skyColor, 0.0, 0.2, vec3(0.8, 1.0, 1.0), vec3(0.5, 0.5, 0.8))
	else if CONDITION_LERP(elevation, skyColor, 0.2, pi / 2, vec3(0.5, 0.5, 0.8), vec3(0.0, 0.4, 0.7))
	
	FragColor = vec4(skyColor, 1.0);
}

