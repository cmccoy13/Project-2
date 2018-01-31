#version 330 core
layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float Scale;

vec3 tempPos;
//uniform vec4 Center;

void main()
{
	tempPos = vertPos;

	if(tempPos.z == 1.0)
	{
		tempPos.x *= Scale;
		tempPos.y *= Scale;
	}

	gl_Position = P * V * M * vec4(tempPos, 1.0);
}
