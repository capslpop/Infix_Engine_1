#pragma once

// Side note:
// when calculated the physcis there is no way to have more than 32 textures in the shaders at once
// so it is important to group all the textures in chunks
// you can still use as much or as littel texture data as you want per chunk
// however this garintees that your charachter is standing in a chunk and can colid with it
// and this way it is a lot easier to load objects in and out of the visible range of your charachter
// but this may not work well with 

//****************************************************************************************
// VOXEL PHYSICS - 
//****************************************************************************************

class VoxelPhysics
{
public:
	VoxelPhysics();
	void init();
	void unpatePhysics();
	~VoxelPhysics();

private:
	std::vector<VoxelChunk> voxelchunks; // keeps a list of all the voxel chunks to itterate through
};

VoxelPhysics::VoxelPhysics()
{

}

void VoxelPhysics::init()
{
	
}

void VoxelPhysics::unpatePhysics()
{
	// Bind shader program
	// Bind each texture in for loop to be exicuted through shader
	// use voxelchunks to itterate through all the textures
	// double check that no textue data is being sent to the GPU
}

VoxelPhysics::~VoxelPhysics(){}

//****************************************************************************************
// VOXEL CHUNK - creates the 3D volume physics objects or uses existing textures
//****************************************************************************************

// MABEY scrap class because it would be harder to access all the memory at once in code?

class VoxelChunk
{
public:
	VoxelChunk();
	void createchunk(int size_x, int size_y, int size_z, int x, int y, int z); // creates voxel texture  // also add to render loop
	void createchunk(GLuint texture_id); // uses current voxel texture
	~VoxelChunk();

private:

};

VoxelChunk::VoxelChunk()
{

}

void VoxelChunk::createchunk(int size_x, int size_y, int size_z, int x, int y, int z)
{

}

void VoxelChunk::createchunk(GLuint texture_id)
{

}

VoxelChunk::~VoxelChunk(){}