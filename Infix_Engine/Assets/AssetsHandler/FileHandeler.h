#pragma once

//************************************************************************************************************************
// GLFW Drop FIle loader and Handler
//************************************************************************************************************************

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

	FileHandeler::_instances.push_back(this); // push the class's instances onto the stack 
}

void FileHandeler::drop_call(const char* path)
{

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