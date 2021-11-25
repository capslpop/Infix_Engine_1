#version 330 core

in vec3 TexCoord;
in vec3 FragPos;
flat in int TexIndex;
flat in vec3 voxel_model_size;

layout(location = 0) out vec4 fragColor;

// Uniform Variables
uniform sampler1D Pallete;
uniform sampler3D u_Textures[31]; // this is equal to toal textures -1 because palette has to be here // TODO: add support for variable textures

uniform vec3 cam_pos;

//**********
// Discard fragments from back faces!!!!  just disable back faced rendering may work? also just make it so backs of faces are olny rendered for the obejct you are currently in?!
//*********

//cosnt vec3 sunDir = vec3(0.9230769, -0.2307692308, -0.3076923);

void main()
{
	vec3 rayDir;
	vec3 rayPos;

	if (gl_FrontFacing)
	{
		rayDir = FragPos - cam_pos; // This finds the direction vector - doing length(raDir) will find the length of the ray from the camera to the box that it intersects  
		rayPos = TexCoord * voxel_model_size;
	} else 
	{
		//discard;
		rayDir = FragPos - cam_pos;  
		rayPos = (cam_pos + voxel_model_size)/2.0; // TODO: fix only being able to fly throught the origin object!!!
	}
	
	vec3 mapPos = floor(rayPos);
	
	vec3 deltaDist = abs(1.0 / rayDir); // ??????????
	
	vec3 rayStep = sign(rayDir);
	
	vec3 sideDist = (rayStep * (mapPos - rayPos) + (rayStep * 0.5) + 0.5) * deltaDist; // first itteration and alignment
	
	vec3 mask;
	
	vec4 tex = texelFetch(u_Textures[TexIndex], ivec3(mapPos), 0);
	
	// This is the main marching loop it only exits if the ray touches the bouning box or it hits a textured cell
	while(tex.r == 0.0)
	{
		if(any(greaterThan(mapPos, voxel_model_size)) || any(lessThan(mapPos, vec3(0.0))))
		{
			discard;
			break;  //TODO find out why needed
		}
	    mask = vec3( lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy)) ); // cheack if you can do multiple jumpes in one itteration
		sideDist += mask * deltaDist;
		mapPos += mask * rayStep;
		tex = texelFetch(u_Textures[TexIndex], ivec3(mapPos), 0);
	}

	fragColor.rgb = texture(Pallete, tex.x).rgb;
	//fragColor.gb = vec2(1.0);
	//fragColor.rgb = vec3(tex.r);
	//fragColor.rgb = vec3(1.0);
}