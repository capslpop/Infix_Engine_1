#version 330 core

in vec3 TexCoord;
in vec3 FragPos;


out vec4 fragColor;

uniform sampler2D ourTexture;

uniform sampler3D nature;
uniform sampler1D natureP;

uniform sampler3D input_tex;
uniform sampler1D input_texP;


uniform float slider;
uniform vec3 cam_pos;
uniform vec2 resolution;
uniform vec3 box_size;

void main()
{
	if (gl_FrontFacing)
	{
		vec3 rayDir = FragPos - cam_pos; // This finds the direction vector - doing length(raDir) will find the length of the ray from the camera to the box that it intersects  
		vec3 rayPos = TexCoord * box_size;

		vec3 mapPos = floor(rayPos);

		vec3 deltaDist = abs(1.0 / rayDir); // ??????????
		
		vec3 rayStep = sign(rayDir);

		vec3 sideDist = (rayStep * (mapPos - rayPos) + (rayStep * 0.5) + 0.5) * deltaDist; // first itteration and alignment
		
		vec3 mask;

		vec4 tex = texelFetch(nature, ivec3(mapPos), 0); // first itteration of ray step march

		// This is the main marching loop it only exits if the ray touches the bouning box or it hits a textured cell
		while(tex.r == 0.0)
		{
			if(any(greaterThan(mapPos, box_size)) || any(lessThan(mapPos, vec3(0))))
			{
				discard;
				break;
			}


		    mask = vec3( lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy)) );		
			sideDist += mask * deltaDist;
			mapPos += mask * rayStep;
			tex = texelFetch(nature, ivec3(mapPos), 0);
		}
		
		fragColor = texture(natureP, tex.r); // vec4(mask, 0.0); //+ vec4(0.1, 0.1, 0.1, 0.0); //+ vec4(mask/10.0, 0.0); //+ 8 * (mask.r+(mask.g/2.0)+(mask.b/3.0))/10.0;
	} else
	{
		//******************************************************************************************************************************
		// Rendering the back faces of triangles
		//******************************************************************************************************************************

		vec3 rayDir = FragPos - cam_pos; // This finds the direction vector - doing length(raDir) will find the length of the ray from the camera to the box that it intersects  
		vec3 rayPos = TexCoord * box_size;

		vec3 mapPos = floor(rayPos);

		vec3 deltaDist = abs(1.0 / rayDir); // ??????????
		
		vec3 rayStep = sign(rayDir);

		vec3 sideDist = (rayStep * (mapPos - rayPos) + (rayStep * 0.5) + 0.5) * deltaDist; // first itteration and alignment
		
		vec3 mask;

		vec4 tex = texelFetch(nature, ivec3(mapPos), 0); // first itteration of ray step march

		// This is the main marching loop it only exits if the ray touches the bouning box or it hits a textured cell
		while(tex.r == 0.0)
		{
			if(any(greaterThan(mapPos, box_size)) || any(lessThan(mapPos, vec3(0))))
			{
				discard;
				break;
			}


		    mask = vec3( lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy)) );		
			sideDist += mask * deltaDist;
			mapPos += mask * rayStep;
			tex = texelFetch(nature, ivec3(mapPos), 0);
		}
		
		fragColor = texture(natureP, tex.r); // vec4(mask, 0.0); //+ vec4(0.1, 0.1, 0.1, 0.0); //+ vec4(mask/10.0, 0.0); //+ 8 * (mask.r+(mask.g/2.0)+(mask.b/3.0))/10.0;
	}
}