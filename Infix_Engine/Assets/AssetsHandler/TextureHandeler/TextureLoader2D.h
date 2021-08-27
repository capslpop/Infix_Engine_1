#pragma once

class TextureLoader2D
{
public:
	TextureLoader2D();
	void CreateTexture(const char* file_path, GLuint shaderID, const char* texture_name);
    void CreateTextureXRAW(const char* file_path, GLuint shaderID, const char* texture_name);
	~TextureLoader2D();

private:
    unsigned int textureID;
};

TextureLoader2D::TextureLoader2D()
{
}

void TextureLoader2D::CreateTexture(const char* file_path, GLuint shaderID, const char* texture_name)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(file_path, &width, &height, &nrChannels, 0);


    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    



    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    //ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(shaderID, texture_name), 0);
    // or set it via the texture class
    //ourShader.setInt("texture2", 1);
    //****************
}

void TextureLoader2D::CreateTextureXRAW(const char* file_path, GLuint shaderID, const char* texture_name)
{
    if (!shaderID)
    {
        printf("failed to initialize shader ID\n");
    }

    FILE* pFile;
    fopen_s(&pFile, file_path, "r");

    if (NULL == pFile) {
        printf("cannot open 2D texture path");
    }



    //fread(&texture_data, 1, sizeof(XRAWformat), pFile);
    //this->printfileinfo();


    int size = 5 * 5;
    GLubyte* pVolume = new GLubyte[size];
    fseek(pFile, sizeof(XRAWformat), SEEK_SET);


    fread(pVolume, sizeof(GLubyte), size, pFile);
    fclose(pFile);

    /*
    printf("size %i\n", size);
    for (int i = 0; i <= size; i++)
    {
        printf("%i: ", i);

        char a = pVolume[i];
        std::bitset<8> x(a);
        std::cout << x << '\n';
    }
    printf("done");
    */
    
    unsigned int textureID;

    //load data into a 3D texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if (pVolume)
    {
        //GL_INTENSITY -- GL_LUMINANCE
        glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY8, 5, 5, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pVolume);
    }
    else
    {
        printf("Failed to load volume form file: %s", texture_name);
    }

    delete[] pVolume;




    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    //ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(shaderID, texture_name), 0);
    // or set it via the texture class
    //ourShader.setInt("texture2", 1);
    //****************
}

TextureLoader2D::~TextureLoader2D()
{
}