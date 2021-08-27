#pragma once

struct XRAWformat {
    char f[4];   //should say XRAW
    unsigned char color_channel_data_type;
    unsigned char num_color_channels;
    unsigned char bits_perchannel;
    unsigned char bits_per_index;
    int width;
    int height;
    int depth;
    int num_palette_clr;
};

class TextureLoader3D
{
public:
    TextureLoader3D();
    bool CreateTexture(const char* file_path, GLuint shaderID, const char* texture_name);
    void printfileinfo();
    ~TextureLoader3D();

    XRAWformat texture_data;

private:
    unsigned int textureID_vol;
    unsigned int textureID_palet;
};

TextureLoader3D::TextureLoader3D()
{
}

bool TextureLoader3D::CreateTexture(const char* file_path, GLuint shaderID, const char* texture_name)
{
    FILE* pFile = fopen(file_path, "rb");
    if (NULL == pFile) {
        printf("could not open file: %s\n", file_path);
        return false;
    }

    fread(&texture_data, 1, sizeof(XRAWformat), pFile);
    this->printfileinfo();

    int size = texture_data.width * texture_data.height * texture_data.depth;

    GLubyte* pVolume = new(std::nothrow) GLubyte[size];
    if (pVolume == nullptr) { printf("could not load volume data from file: %s\n", file_path); return false; }

    fseek(pFile, sizeof(XRAWformat), SEEK_SET); // Set read pos to be after the header for the file
    fread(pVolume, sizeof(GLubyte), size, pFile); // Read voxel data into pVolume


    // todo: fix pallette loading!
    const int total_pallette_col = 256; // 256 colors * 4 bytes in one color
    GLubyte* pPalette = new(std::nothrow) GLubyte[total_pallette_col * 4]; // allocate 256 colors with 32 bit depth to be used for the palette 
    if (pPalette == nullptr) { printf("could not load pallette data from file: %s\n", file_path); return false; }
    fseek(pFile, sizeof(XRAWformat) + size, SEEK_SET); // Set read pos to be after volume data
    fread(pPalette, sizeof(GLubyte), total_pallette_col * 4, pFile); // read palette data from file into pPalette

    fclose(pFile);

    //load data into a 3D texture
    glGenTextures(1, &textureID_vol);
    glBindTexture(GL_TEXTURE_3D, textureID_vol);

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // fixes allignment so that texture's width - height - depth can be any size 
    glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY8, texture_data.width, texture_data.height, texture_data.depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pVolume);
    delete[] pVolume;
    // send data to shaders:
    glUniform1i(glGetUniformLocation(shaderID, texture_name), 0);


    //load data into a 2d palette texture
    glGenTextures(1, &textureID_palet);
    glBindTexture(GL_TEXTURE_1D, textureID_palet);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, total_pallette_col, 0, GL_RGBA, GL_UNSIGNED_BYTE, pPalette);
    delete[] pPalette;
    // send data to shaders:
    glUniform1i(glGetUniformLocation(shaderID, texture_name + 'P'), 0);
    
    
    glUniform3f(glGetUniformLocation(shaderID, "box_size"), texture_data.width, texture_data.height, texture_data.depth);

    return true;
}



void TextureLoader3D::printfileinfo()
{

    printf("FILE TYPE = %s \n",                texture_data.f);
    printf("Color Channel Data Type = %i \n",  texture_data.color_channel_data_type);
    printf("Number of Color Channels = %i \n", texture_data.num_color_channels);
    printf("Bits Per channel = %i \n",         texture_data.bits_perchannel);
    printf("Bits Per Index = %i \n",           texture_data.bits_per_index);

    printf("width = %i \n",                    texture_data.width );
    printf("height = %i\n",                    texture_data.height);
    printf("depth = %i \n",                    texture_data.depth );
    printf("Number Pallette Color = %i \n",    texture_data.num_palette_clr);

}

TextureLoader3D::~TextureLoader3D()
{
}