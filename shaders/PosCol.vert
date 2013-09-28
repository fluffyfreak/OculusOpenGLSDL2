#version 330

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec4 vertexCol;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

out vec4 color;

void main()
{
	color = vertexCol;
	gl_Position = MVP * vec4(vertexPos,1.0);
} 
