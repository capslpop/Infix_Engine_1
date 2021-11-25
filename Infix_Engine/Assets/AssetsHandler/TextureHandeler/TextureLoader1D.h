#pragma once

class TextureLoader1D
{
public:
	TextureLoader1D();
	void LaodPNG(const char* file_path, GLuint program_id);
	~TextureLoader1D();
private:

};

TextureLoader1D::TextureLoader1D()
{
}

void TextureLoader1D::LaodPNG(const char* file_path, GLuint program_id)
{
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load(file_path, &width, &height, &nrChannels, 0);

	GLuint textureID_palet;
	glGenTextures(1, &textureID_palet);
	glBindTexture(GL_TEXTURE_1D, textureID_palet);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAX_LEVEL, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);


	glUniform1i(glGetUniformLocation(program_id, "Pallete"), 0);
}

TextureLoader1D::~TextureLoader1D()
{
}

