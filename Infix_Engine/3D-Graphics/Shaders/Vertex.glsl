#version 330 core

layout (location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec3 aTexCoord;
layout (location = 2) in float aTexIndex;
layout (location = 3) in vec3 avoxel_model_size;

uniform mat4 camera_mat4;

out vec3 TexCoord;
out vec3 FragPos;
flat out int TexIndex;
flat out vec3 voxel_model_size;

void main()
{ 	
	gl_Position.xyzw = camera_mat4 * vec4(vertexPosition_modelspace, 1.0);

    TexCoord = aTexCoord;
	TexIndex = int(aTexIndex);
	FragPos = vertexPosition_modelspace;
	voxel_model_size = avoxel_model_size;
}
