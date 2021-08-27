#version 330 core

layout (location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec3 aTexCoord;

uniform mat4 camera_mat4;

out vec3 TexCoord;
out vec3 FragPos;

void main()
{ 	
	gl_Position.xyzw = camera_mat4 * vec4(vertexPosition_modelspace, 1.0);

    TexCoord = aTexCoord;
	FragPos = vertexPosition_modelspace;
}