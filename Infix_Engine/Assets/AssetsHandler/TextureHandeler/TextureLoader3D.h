#pragma once

class TextureLoader3D
{
public:
    TextureLoader3D();
    bool CreateTextureXRAW(const char* file_path, GLuint shaderID, const char* texture_name, int& num_bind_tex_unit, DrawWindow &drawwindow);
    std::vector<GLuint> CreateTextureVOX(const char* file_path, int& num_bind_tex_unit, DrawWindow &drawwindow, GLuint program_id);
    ~TextureLoader3D();

    unsigned int palette_IMGUI[256];
    GLuint pallete_id;
private:
    static GLuint Bind3Dtexture(int size_x, int size_y, int size_z, const void* Volume, int& num_bind_tex_unit);
    static void Bind1DPalette(const void* palette, GLuint& pallete_id);

    //************************************************************************************************************
    // XRAW FORMAT - this is the raw 3d texture data + header format
    //************************************************************************************************************
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

    //************************************************************************************************************
    // VOX FORMAT - this is magicavoxel's custom file fomrat .vox
    //************************************************************************************************************
    struct VOXformat
    {
        char type[4]; // should be 4 char that = "VOX " - include space
        int version_num;
        char main[4]; // this just hold the main text in the variable
        int numBytesMainChunk;
        int numBytesMainChildrenChunks;
    };

    struct VOXChunkFormatSIZE
    {
        //char size[4]; // should just spell out size // still in file just not using this struct to read the data
        int numBytesChunkSIZE;
        int numBytesChildChunkSIZE;
        int size_x;
        int size_y;
        int size_z;
    };

    struct VOXChunkFormatXYZI
    {
        char xyzi[4]; // should just spell out xyzi 
        int numBytesChunkXYZI;
        int numBytesChildChunkXYZI;
        int numVoxels; // total number of voxels
        // then (x, y, z, colorIndex) : 1 byte for each component
    };

    struct VOXDataFormat
    {
        GLubyte x;
        GLubyte y;
        GLubyte z;
        GLubyte colorIndex;
    };

    struct VOXfileafter
    {
        int numBytesChunk;
        int numBytesChildChunk;
    };
};

TextureLoader3D::TextureLoader3D()
{
}

bool TextureLoader3D::CreateTextureXRAW(const char* file_path, GLuint shaderID, const char* texture_name, int &num_bind_tex_unit, DrawWindow &drawwindow)
{
    std::ifstream file_3D_tex;
    file_3D_tex.open(file_path, std::ios::in | std::ios::binary);
    if (!file_3D_tex.is_open()) { printf("Could not open file: %s\n", file_path); return false; }
    
    XRAWformat texture_data_xraw;
    file_3D_tex.read((char*)&texture_data_xraw, sizeof(XRAWformat));
    //printf("FILE TYPE = %s \n", texture_data_xraw.f);
    //printf("Color Channel Data Type = %i \n", texture_data_xraw.color_channel_data_type);
    //printf("Number of Color Channels = %i \n", texture_data_xraw.num_color_channels);
    //printf("Bits Per channel = %i \n", texture_data_xraw.bits_perchannel);
    //printf("Bits Per Index = %i \n", texture_data_xraw.bits_per_index);
    //printf("width = %i \n", texture_data_xraw.width);
    //printf("height = %i\n", texture_data_xraw.height);
    //printf("depth = %i \n", texture_data_xraw.depth);
    //printf("Number Pallette Color = %i \n", texture_data_xraw.num_palette_clr);

    int size = texture_data_xraw.width * texture_data_xraw.height * texture_data_xraw.depth;

    GLubyte* pVolume = new(std::nothrow) GLubyte[size];
    if (pVolume == nullptr) { printf("could not load volume data from file: %s\n", file_path); return false; }

    file_3D_tex.seekg(sizeof(XRAWformat), std::ios::beg);  // Set read pos to be after the header for the file
    file_3D_tex.read((char*)pVolume, size); // Read voxel data into pVolume

    // Pallette Loading
    const int total_pallette_col = 256; // 256 colors * 4 bytes in one color
    GLubyte* pPalette = new(std::nothrow) GLubyte[total_pallette_col * 4]; // allocate 256 colors with 32 bit depth to be used for the palette 
    if (pPalette == nullptr) { printf("could not load pallette data from file: %s\n", file_path); return false; }

    file_3D_tex.seekg(sizeof(XRAWformat) + size, std::ios::beg); // Set read pos to be after volume data
    file_3D_tex.read((char*)pPalette, total_pallette_col * 4); // read the pallette data into pPalette
    file_3D_tex.close();

    //load data into a 3D texture
    GLuint textureID_vol;
    textureID_vol = Bind3Dtexture(texture_data_xraw.width, texture_data_xraw.height, texture_data_xraw.depth, pVolume, num_bind_tex_unit);
    delete[] pVolume;

    // now add geometry
    drawwindow.Addgeometry(0, 0, 0, texture_data_xraw.width, texture_data_xraw.height, texture_data_xraw.depth, num_bind_tex_unit);

    Bind1DPalette(pPalette, shaderID);
    delete[] pPalette;

    return true;
}

std::vector<GLuint> TextureLoader3D::CreateTextureVOX(const char* file_path, int& num_bind_tex_unit, DrawWindow &drawWindow, GLuint program_id)
{
    // This reads .vox files into 3Dtexture format for Opengl. 
    // The way this works is .vox files store each voxel with their position (X, Y Z). 
    // This takes those positions and transformes them into a 1D array of contiuse data.

    printf("Loading file: %s\n", &file_path[0]);

    // all texture ID's :
    std::vector<GLuint> texture_ids;

    // store the value of num_bind_tex_units for latter
    int old_num_bind_tex_unit = num_bind_tex_unit;

    unsigned int palette[256]; // this is the current palette being used

    // Open file
    std::ifstream file;
    file.open(file_path, std::ios::in | std::ios::binary);
    if (!file.is_open()) { printerror("Could not open file", file_path); }

     // 1. File Structure : RIFF style
    // -------------------------------------------------------------------------------
    // # Bytes  | Type       | Value
    // -------------------------------------------------------------------------------
    // 1x4      | char       | id 'VOX ' : 'V' 'O' 'X' 'space', 'V' is first
    // 4        | int        | version number : 150
    //
    // Chunk 'MAIN'
    // {
    //     // pack of models
    //     Chunk 'PACK'    : optional
    //
    //     // models
    //     Chunk 'SIZE'
    //     Chunk 'XYZI'
    //
    //     Chunk 'SIZE'
    //     Chunk 'XYZI'
    //
    //     ...
    //
    //     Chunk 'SIZE'
    //     Chunk 'XYZI'
    //     
    //     
    //     // palette
    //     Chunk 'RGBA'    : optional
    // }
    // -------------------------------------------------------------------------------

     // 2. Chunk Structure
    // -------------------------------------------------------------------------------
    // # Bytes  | Type       | Value
    // -------------------------------------------------------------------------------
    // 1x4      | char       | chunk id
    // 4        | int        | num bytes of chunk content (N)
    // 4        | int        | num bytes of children chunks (M)
    // N        |            | chunk content
    // M        |            | children chunks
    // -------------------------------------------------------------------------------

    VOXformat vox;
    file.read((char*)&vox, sizeof(VOXformat));

    // Check file type and version:
    if (vox.type[0] != 'V' && vox.type[1] != 'O' && vox.type[2] != 'X') { printerror("loading wrong file type use .vox rather than", file_path); }
    if (vox.version_num != 150) { printwarning("version number may be unsuported! Please use version number 150 for .vox files. You are using", vox.version_num); }

    std::vector<glm::vec3> msize; // stores the size of the objects
    std::vector<glm::vec3> mpos; // stores the position of objects
    std::vector<int> mmodel_id; // stores which voxel moddel to load for the given transform 
    
    char chunk_id[4];
    while (true)
    {
        file.read(chunk_id, sizeof(chunk_id)); // Read the chunk id for each chunk

        if (chunk_id[0] == 'S' && chunk_id[1] == 'I' && chunk_id[2] == 'Z' && chunk_id[3] == 'E')
        {
            // Read both SIZE and XYZI chunks at the same time because the are garanteed to be together
            VOXChunkFormatSIZE vox_size;
            // 5. Chunk id 'SIZE' : model size
            // -------------------------------------------------------------------------------
            // # Bytes  | Type       | Value
            // -------------------------------------------------------------------------------
            // 4        | int        | size x
            // 4        | int        | size y
            // 4        | int        | size z : gravity direction
            // -------------------------------------------------------------------------------
            file.read((char*)&vox_size, sizeof(VOXChunkFormatSIZE));

            // Store in vecotor to be use latter for making bounding boxes:
            msize.push_back(glm::vec3(vox_size.size_x, vox_size.size_y, vox_size.size_z));


            VOXChunkFormatXYZI vox_xyzi;
            // 6. Chunk id 'XYZI' : model voxels
            // -------------------------------------------------------------------------------
            // # Bytes  | Type       | Value
            // -------------------------------------------------------------------------------
            // 4        | int        | numVoxels (N)
            // 4 x N    | int        | (x, y, z, colorIndex) : 1 byte for each component
            // -------------------------------------------------------------------------------
            file.read((char*)&vox_xyzi, sizeof(vox_xyzi));

            //printf("Number of Bytes for Chunk: %i  \n", size.numBytesChunkSIZE);
            //printf("Number of Bytes for Child Chunk: %i  \n", size.numBytesChildChunkSIZE);
            //printf("size x: %i  \n", size.size_x);
            //printf("size y: %i  \n", size.size_y);
            //printf("size z: %i  \n", size.size_z);
            //printf("Number of Bytes for Chunk: %i  \n", xyzi.numBytesChunkXYZI);
            //printf("Number of Bytes for Child Chunk: %i  \n", xyzi.numBytesChildChunkXYZI);
            //printf("Total number of voxels: %i\n", xyzi.numVoxels);

            int size = vox_size.size_x * vox_size.size_y * vox_size.size_z;

            GLubyte* pVolume = new(std::nothrow) GLubyte[size](); // () is very important because it initializes the data to all 0s which means that are air voxels
            if (pVolume == nullptr) { printerror("Could not load volume data for file", file_path); }
            
            // Precompute offsets
            float offset_one = vox_size.size_y * vox_size.size_z;
            float offset_two = vox_size.size_z;

            for (int i = 0; i < vox_xyzi.numVoxels; i++)
            {
                VOXDataFormat vox_color;
                // (x, y, z, colorIndex) : 1 byte for each component
                file.read((char*)&vox_color, sizeof(VOXDataFormat));

                // Read the voxel data into the same format as the 3DTexture for Opengl textures
                // By taking the x, y, and z positions of the voxels we can insert them into their correct places in the 1D pVolume.
                //int offset = vox_color.x + vox_size.size_x * (vox_size.size_z - vox_color.z - 1) + vox_size.size_x * vox_size.size_z * vox_color.y;
                
                int offset = vox_color.x * vox_size.size_y * vox_size.size_z + (vox_size.size_y - vox_color.y - 1) * vox_size.size_z + vox_color.z;
                //int offset = vox_color.x * vox_size.size_y * vox_size.size_z + vox_color.z * vox_size.size_y + vox_color.y;
                //int offset = vox_color.y * vox_size.size_x * vox_size.size_z + vox_color.x * vox_size.size_z + vox_color.z;
                //int offset = vox_color.y * vox_size.size_x * vox_size.size_z + vox_color.z * vox_size.size_x + vox_color.x;
                //int offset = vox_color.z * vox_size.size_x * vox_size.size_y + vox_color.x * vox_size.size_y + vox_color.y;
                //int offset = vox_color.z * vox_size.size_x * vox_size.size_y + vox_color.y * vox_size.size_x + vox_color.x;

                *(pVolume + offset) = vox_color.colorIndex - 1; // Index is between 0 - 254 because there are no air blocks that are stored here.
            }
            texture_ids.push_back(this->Bind3Dtexture(vox_size.size_z, vox_size.size_y, vox_size.size_x, pVolume, num_bind_tex_unit));

            delete[] pVolume;

            int file_loaction = file.tellg();

            updateloading((float)file_loaction / (float)vox.numBytesMainChildrenChunks);
            printf(" %i of %i", file_loaction, vox.numBytesMainChildrenChunks);

            
        } else if (chunk_id[0] == 'R' && chunk_id[1] == 'G' && chunk_id[2] == 'B' && chunk_id[3] == 'A')
        {
            // 7. Chunk id 'RGBA' : palette
            // -------------------------------------------------------------------------------
            // # Bytes  | Type       | Value
            // -------------------------------------------------------------------------------
            // 4 x 256  | int        | (R, G, B, A) : 1 byte for each component
            //                       | * <NOTICE>
            //                       | * color [0-254] are mapped to palette index [1-255], e.g :
            //                       |
            //                       | for ( int i = 0; i <= 254; i++ ) {
            //                       |     palette[i + 1] = ReadRGBA();
            //                       | }
            // -------------------------------------------------------------------------------
            VOXfileafter file_rgab;
            file.read((char*)&file_rgab, sizeof(VOXfileafter));
            file.read((char*)palette, sizeof(palette));
        } else if (chunk_id[0] == 'I' && chunk_id[1] == 'M' && chunk_id[2] == 'A' && chunk_id[3] == 'P')
        {
            // Skip this chunk
            VOXfileafter imap;
            file.read((char*)&imap, sizeof(VOXfileafter));
            GLubyte imap_indexs[256];
            file.read((char*)imap_indexs, sizeof(imap_indexs));

            // creat a old palette to load the data out of
            //unsigned int old_palette[256];
            //for (int i = 0; i < 256; i++) 
            //{
            //    old_palette[i] = palette[i];
            //}
            //
            //// now load all of old_palette into palette using the indacies offset from IMAP
            //for (int i = 0; i < 256; i++)
            //{
            //    palette[imap_indexs[i]] = old_palette[i];
            //}
            
        } else if (chunk_id[0] == 'M' && chunk_id[1] == 'A' && chunk_id[2] == 'T' && chunk_id[3] == 'L')
        {
            // Skip this chunk
            VOXfileafter matl;
            file.read((char*)&matl, sizeof(VOXfileafter));
            file.seekg(matl.numBytesChunk, std::ios::cur);
        }else if (chunk_id[0] == 'L' && chunk_id[1] == 'A' && chunk_id[2] == 'Y' && chunk_id[3] == 'R')
        {
            // Skip this chunk
            VOXfileafter file_after;
            file.read((char*)&file_after, sizeof(file_after));
            file.seekg(file_after.numBytesChunk, std::ios::cur);
        }
        else if (chunk_id[0] == 'n' && chunk_id[1] == 'T' && chunk_id[2] == 'R' && chunk_id[3] == 'N')
        {
            // This chunk stores the position of the moddel most importantly. 
            //However, there other than the position data there is nothing we need here.
            VOXfileafter file_after;
            file.read((char*)&file_after, sizeof(VOXfileafter));

            if (file_after.numBytesChunk == 28) // since the first chunk is always 28 bytes and all normal nTRN chunks are all at least 32 bytes this works
            {
                // this nTRN chunk is master chunk that we don't need to use 
                // so skip over this chunk
                file.seekg(file_after.numBytesChunk, std::ios::cur);
            }
            else
            {

            int chunk_info_one[2]; // this reads both the node_id and the hidden node at the same time
            file.read((char*)&chunk_info_one, sizeof(chunk_info_one));
           
            if (chunk_info_one[1] == 1) // if the hidden node is turned on then that means that the _name chunk is present and we have to skip over it
            {
                
                char name_chunk[4 + 5]; // skip the first 4 bytes 
                file.read((char*)&name_chunk, sizeof(name_chunk)); // up the seek pointer after the _name chunck id

                int name_string_size;
                file.read((char*)&name_string_size, sizeof(int));
                file.seekg(name_string_size, std::ios::cur);
            }

            int chunk_info_two[6];
            file.read((char*)&chunk_info_two, sizeof(chunk_info_two));

            // CHUNK FORMAT
            // chunk_info_one[0] = "node id"   
            // chunk_info_one[1] = "hidden node"
            // possible _name chunk that needs to be read
            // chunk_info_two[0] = "child node"       
            // chunk_info_two[1] = "reserved id(-1)" -- this should always be -1
            // chunk_info_two[2] = "layer id"        
            // chunk_info_two[3] = "number of frames"    
            // chunk_info_two[4] = "Unknown one"      
            // chunk_info_two[5] = "Unknown two"         

            // Read in the smaller chunk id. It is ether _r or _t
            char attributes_id[2];
            file.read((char*)&attributes_id, sizeof(attributes_id));
            if (attributes_id[0] == '_' && attributes_id[1] == 'r')
            {
                int rot_num_char;
                file.read((char*)&rot_num_char, sizeof(int));
                //printf("number of chars for _r: %i ", rot_num_char);

                char* string_rot = new char[rot_num_char];
                file.read(string_rot, rot_num_char);
                //printf("string rot: %s \n", string_rot);
                delete[] string_rot;

                // set the positon of the cursor after the rotation byte using the number of charachters in a string + intager(4 bytes) + 2 bytes for _t chunk
                //file.seekg(rot_num_char + 4 + 2, std::ios::cur);
                file.seekg(4 + 2, std::ios::cur);
            }

            // Read total position length of the string
            int total_pos_length;
            file.read((char*)&total_pos_length, sizeof(int));
            // Read the string of 3 positions in all seperated by spaces. Example: "-987 2 67"
            char* pos_as_str = new char[total_pos_length];
            file.read(pos_as_str, total_pos_length);
            
            glm::vec3 pos_xyz;
            // X coordinate
            pos_xyz.x = std::stoi(pos_as_str);

            // Y and Z coordinate
            bool is_y = true;
            for (int i = 0; i < total_pos_length; i++)
            {
                if (pos_as_str[i] == ' ')
                {
                    if (is_y)
                    {
                        pos_xyz.y = std::stoi(&pos_as_str[i + 1]);
                        is_y = false;
                    } 
                    else
                    {
                        pos_xyz.z = std::stoi(&pos_as_str[i + 1]);
                    }
                }
            }
            delete[] pos_as_str;
            mpos.push_back(pos_xyz);
            }
        } else if (chunk_id[0] == 'n' && chunk_id[1] == 'G' && chunk_id[2] == 'R' && chunk_id[3] == 'P')
        {
            // Skip this chunk
            VOXfileafter file_after;
            file.read((char*)&file_after, sizeof(VOXfileafter));
            file.seekg(file_after.numBytesChunk, std::ios::cur);
        } else if (chunk_id[0] == 'n' && chunk_id[1] == 'S' && chunk_id[2] == 'H' && chunk_id[3] == 'P')
        {
            // Skip this chunk
            VOXfileafter file_after;
            file.read((char*)&file_after, sizeof(VOXfileafter));

            int chunk_info_one[2]; // this reads both the chunk node_id and the number of key-values pair
            file.read((char*)&chunk_info_one, sizeof(int) * 2);

            if (chunk_info_one[1] > 0) {
                for (int i = 0; i < chunk_info_one[1] * 2; i++) { // skips DICT scesion of cunk
                    int size_of_string;
                    file.read((char*)&size_of_string, sizeof(int));
                    file.seekg(size_of_string, std::ios::cur);
                }
            }

            int number_of_moddels;
            file.read((char*)&number_of_moddels, sizeof(int));

            if (number_of_moddels != 1) {
                printerror("number of moddels in the nSHP chunk is more than 1 but assumes 1.");
            }

            // model_id refrences the 3D texture that nTRN chunk transforms
            int model_id;
            file.read((char*)&model_id, sizeof(int));

            mmodel_id.push_back(model_id); // load the current model id into the array

            int size_of_string_one;
            file.read((char*)&size_of_string_one, sizeof(int));
            file.seekg(size_of_string_one, std::ios::cur);
        }
        else
        {
            // if none of the chunks are found in this loop then it finished loading the file.
            break;
        }

    }
    file.close();
    
    // Load all of the textures geometry into the vertex data
    for (int i = 0; i < mpos.size(); i++)
    {
        // Flip z and y positions to convert to Opengl coords
        int k = mmodel_id[i];
        //printf("This is moddel: %i: %i, %i, %i\t\tsize: %i, %i, %i\n", i, (int)mpos[i].x, (int)mpos[i].z, (int)mpos[i].y, (int)msize[k].x, (int)msize[k].y, (int)msize[k].z);
        // TODO: fix alignment with model_id not counting consecutivly (use hash map to reoganize the values with respective position values) 
        drawWindow.Addgeometry(mpos[i].x, -mpos[i].z, mpos[i].y, msize[k].z, msize[k].y, msize[k].x, old_num_bind_tex_unit + k);
    }

    // Now load the palette
    Bind1DPalette(palette, pallete_id);
    glUseProgram(program_id);
    glUniform1i(glGetUniformLocation(program_id, "Pallete"), 0);

    //glActiveTexture(GL_TEXTURE31);
    //glBindTexture(GL_TEXTURE_3D, program_id);

    // this is temporary
    for (int i = 0; i < 256; i++)
    {
        palette_IMGUI[i] = palette[i];
    }

    return texture_ids;
}

GLuint TextureLoader3D::Bind3Dtexture(int size_x, int size_y, int size_z, const void* Volume, int & num_bind_tex_unit)
{
    //load data into a 3D texture
    GLuint textureID_vol;
    glGenTextures(1, &textureID_vol);
    glBindTexture(GL_TEXTURE_3D, textureID_vol);

    //Once bound we can itterate the bind counter by one
    num_bind_tex_unit++;

    // set the texture parameters
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if (size_x % 4 == 0) // if divisible by 4 then load the texture with standard alignment
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
    else // else load the texture with an alignment of 1 so that the texture doesn't become shifted
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY8, size_x, size_y, size_z, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, Volume);
    //glCompressedTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY8, size_x, size_y, size_z, 0, 256*256*256, Volume);

    return textureID_vol;
}

void TextureLoader3D::Bind1DPalette(const void* palette, GLuint &pallete_id) // sends the current palette to the GPU
{
    glGenTextures(1, &pallete_id);
    glBindTexture(GL_TEXTURE_1D, pallete_id);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, palette);
}

TextureLoader3D::~TextureLoader3D()
{

}