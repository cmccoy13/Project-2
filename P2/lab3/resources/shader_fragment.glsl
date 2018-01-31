#version 330 core
out vec3 color;

in vec4 gl_FragCoord;
uniform vec4 Center;

void main()
{
	color = vec3(1.0f, 0.0f, 0.0f);
	float dist = distance(gl_FragCoord, Center);

	if(dist < 50)
	{
		color = vec3(1.0, ((211.0+dist)/255.0), ((41.0+dist)/255.0));
	}
	else
	{
	
	}
}
