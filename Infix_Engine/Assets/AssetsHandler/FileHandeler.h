#pragma once


class FileHandeler
{
public:
	FileHandeler(GLFWwindow* window, GLuint shader_id);
	~FileHandeler();

private:
	void drop_call(const char* path);
	static void drop_callback(GLFWwindow* window, int count, const char** paths);

	//Variables:
	GLuint shaderID;
	static std::vector<FileHandeler*> _instances; // creates a list of all the instances of the class
};

std::vector<FileHandeler*> FileHandeler::_instances;

FileHandeler::FileHandeler(GLFWwindow* window, GLuint shader_id)
{
	glfwSetDropCallback(window, FileHandeler::drop_callback);
	shaderID = shader_id;

	FileHandeler::_instances.push_back(this); // push the classes instances onto the stack 
}

void FileHandeler::drop_call(const char* path)
{
	//TextureLoader3D tex;
	//tex.CreateTexture(path, shaderID, "input_tex"); //TODO: add asset handler with an aray of 3d textures for continualy storing textures!
	//TODO: textures are created indefinatly so add a way to destroy them once the next file is droped
}

FileHandeler::~FileHandeler() {}


void FileHandeler::drop_callback(GLFWwindow* window, int count, const char** paths)
{
    int i;
	for (i = 0; i < count; i++)
	{
		for (FileHandeler* file_handeler : _instances) //iterate through all instances of of FileHandeler
		{
			file_handeler->drop_call(paths[i]);
		}
	}
}