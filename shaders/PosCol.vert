
in vec3 vertexPos;
in vec4 vertexCol;

varying vec4 color;

void main()
{
	color = vertexCol;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(vertexPos,1.0);
} 
