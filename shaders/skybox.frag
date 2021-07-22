
#version 330 core

out vec4 FragColor;  
in vec3 worldDir;

uniform vec3 skyColor_change;
uniform vec3 groundColor_change;
uniform vec3 ladder1;
uniform vec3 ladder2;

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
	if CONDITION_LERP(elevation, skyColor, -pi/2, -0.1, groundColor_change,  groundColor_change)
	else if CONDITION_LERP(elevation, skyColor, -0.1, 0.0, groundColor_change, ladder1)
	else if CONDITION_LERP(elevation, skyColor, 0.0, 0.2, ladder1, ladder2)
	else if CONDITION_LERP(elevation, skyColor, 0.2, pi / 2, ladder2, skyColor_change)
	
	FragColor = vec4(skyColor, 1.0);
}

