#version 430 core

layout(location = 0) in vec4 vPosition;
out vec4 color;
uniform vec4 dominating_color;
uniform mat4 projection_matrix;
uniform mat4 WorldtoCameraMatrix;


void main()
{
    vec4 v = vPosition;
	vec3 L = vec3(0.5f,0.5f, 0.5f);

	vec4 newPosition =     projection_matrix * WorldtoCameraMatrix  *    v;

	gl_Position = newPosition;



	color = dominating_color;
	
}
