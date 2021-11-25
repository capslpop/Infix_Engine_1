#pragma once

int gl_max_tex_slots;



class DrawWindow
{
public:
	DrawWindow();
    GLFWwindow* InitWindow();
    void AddTextureSlots(GLuint program_id_fbo);
    void BindGeometry();
    void Addgeometry(float x, float y, float z, float size_x, float size_y, float size_z, float num_bind_tex_unit);
    void Render(std::vector<GLuint> texture_ids, GLuint pallete_id, GLuint Program_id_fbo, GLuint Program_id_window);
    void Destroy();
	~DrawWindow();

    static void ResizeWindowCallback(GLFWwindow* window, int width, int height);

private:
    GLuint vao, vbo, ibo, rbo, vao_win, vbo_win; // Buffer refrences
    GLuint TextureColorBuffer; // this is the refrence to the frame buffer texture

    int number_of_bount_tex = 0; // this is to find the total number of texture slots that are bound using the BindGeometry() function

    // FBO:
    GLuint fbo;
    int gl_max_tex_slots_for_fbo;
    GLuint u_Textures; // texture samplers for fbo

    std::vector<float> v_vertex_list;
    std::vector<unsigned int> v_index_list;

    std::vector<int> geometry_batch_size; // this stores the total batch size of each of the geometry
};

DrawWindow::DrawWindow()
{
}

static void glfw_error_callback(int error, const char* description)
{
    //fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    printerror(description, error);
}


GLFWwindow* DrawWindow::InitWindow()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        printf("Failed to init GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Voxel Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        printf("Window pointer is NULL");
    }

    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gladLoadGL() == 0;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    }
    
    glfwSetWindowSizeCallback(window, DrawWindow::ResizeWindowCallback);


    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_max_tex_slots);
    gl_max_tex_slots_for_fbo = gl_max_tex_slots - 1; // subtract one value for the uniform pallette texture
    printf("number of texture slots for FBO are: %i \n", gl_max_tex_slots_for_fbo);

    return window;
}

void DrawWindow::AddTextureSlots(GLuint program_id_fbo)
{
    // Generate samplers for fbo
    std::vector<int> samplers;
    u_Textures = glGetUniformLocation(program_id_fbo, "u_Textures");
    for (int k = 0; k < gl_max_tex_slots_for_fbo; k++)
    {
        samplers.push_back(k);
    }
    glUniform1iv(u_Textures, gl_max_tex_slots_for_fbo, &samplers[0]);
}

void DrawWindow::BindGeometry()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // Create buffer and copy data
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v_vertex_list.size() * sizeof(float), &v_vertex_list[0], GL_STATIC_DRAW);
   
    // Create index buffer
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, v_index_list.size() * sizeof(unsigned int), &v_index_list[0], GL_STATIC_DRAW);
    
    int number_of_attributes = 10;
    //**************************************************************************************************
    //              |Size of attributes|      |How many attributes|                 |Offset from start of attributes|
    //Position attribute     |                         |                                         |
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, number_of_attributes * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, number_of_attributes * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // this telles glsl which texture to use
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, number_of_attributes * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // box's width, height, and depth for ray marching into the bounding box
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, number_of_attributes * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(3);
    //***************************************************************************************************
    

    // generate the geometry batch size:
    // assuming batch size 16 in example:
    // ex: 1 1 1 1 2 3 4 5 6 7 8 8 8 8 8 9 10 11 12 13 14 15 16  ||  1 1 2 3 4 5 6 7 ..
    // ex :becomes: 23, (size of nth batch), (size of n+1 batch)
    int batch_size = 0;
    for (int i = 0; i < v_vertex_list.size()/80; i++)
    {
        batch_size++;
        if ((int)v_vertex_list.at(6 + i * (number_of_attributes * 8)) >= (gl_max_tex_slots_for_fbo-1))
        {
            geometry_batch_size.push_back(batch_size);
            batch_size = 0;
        }
    }
    if (batch_size > 0) 
    {
        geometry_batch_size.push_back(batch_size);
    }

    //---------------
    // Frame Buffer
    //---------------
    
    // Create the frame buffer:
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    
    glGenTextures(1, &TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 1280, 720
    int width = 1280, height = 720;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureColorBuffer, 0);

    // TODO: change rbo into changable values for better depth testing with voxels!
    // Create the Render Buffer:
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


    //------------------
    // Draw Screen Quad -- for rendering the frame buffer to
    //------------------
    
    // Creating screen VAO:
    // This create the screen to draw to (specifically for postprocessing effects):
    static float vertex_window[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao_win);
    glBindVertexArray(vao_win);

    glGenBuffers(1, &vbo_win);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_win);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_window), &vertex_window, GL_STATIC_DRAW);
    //**************************************************************************************************
    //              |Size of attributes|  |How many attributes| |Offset from start of attributes|
    //Position attribute     |                         |                      |
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //***************************************************************************************************

    // Render only the front side of the cube
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
}

void DrawWindow::Addgeometry(float x, float y, float z, float size_x, float size_y, float size_z, float num_bind_tex_unit) // Supply x,y,z and size of x,y,z 
//| then for num_bind_tex_unit supply the current refrence to the texture it will automatically batch the geometry together
{
    float offset_bind_tex = num_bind_tex_unit - (gl_max_tex_slots_for_fbo * floor(num_bind_tex_unit / gl_max_tex_slots_for_fbo));

    float sizeX = ceil(size_x / 2); // ceil fixes alignment issues with voxel moddels
    float sizeY = ceil(size_y / 2);
    float sizeZ = ceil(size_z / 2);

    // objects are centered at their center
    v_vertex_list.insert(v_vertex_list.end(), {
        // front                      
        //position                           //texture coords       // texture samplers       // Box's width, height, and depth
        x - sizeX, y - sizeY, z + sizeZ,     0.0, 0.0, 1.0,         offset_bind_tex,          size_x, size_y, size_z,
        x + sizeX, y - sizeY, z + sizeZ,     1.0, 0.0, 1.0,         offset_bind_tex,          size_x, size_y, size_z,
        x + sizeX, y + sizeY, z + sizeZ,     1.0, 1.0, 1.0,         offset_bind_tex,          size_x, size_y, size_z,
        x - sizeX, y + sizeY, z + sizeZ,     0.0, 1.0, 1.0,         offset_bind_tex,          size_x, size_y, size_z,
        // back                                                    
        x - sizeX, y - sizeY, z - sizeZ,     0.0, 0.0, 0.0,         offset_bind_tex,          size_x, size_y, size_z,
        x + sizeX, y - sizeY, z - sizeZ,     1.0, 0.0, 0.0,         offset_bind_tex,          size_x, size_y, size_z,
        x + sizeX, y + sizeY, z - sizeZ,     1.0, 1.0, 0.0,         offset_bind_tex,          size_x, size_y, size_z,
        x - sizeX, y + sizeY, z - sizeZ,     0.0, 1.0, 0.0,         offset_bind_tex,          size_x, size_y, size_z
        });

    unsigned int offset = (int)number_of_bount_tex * 8;

    v_index_list.insert(v_index_list.end(),{
            //First cube
            // 
            // front
            0 + offset, 1 + offset, 2 + offset,
            2 + offset, 3 + offset, 0 + offset,
            // right
            1 + offset, 5 + offset, 6 + offset,
            6 + offset, 2 + offset, 1 + offset,
            // back      
            7 + offset, 6 + offset, 5 + offset,
            5 + offset, 4 + offset, 7 + offset,
            // left      
            4 + offset, 0 + offset, 3 + offset,
            3 + offset, 7 + offset, 4 + offset,
            // bottom    
            4 + offset, 5 + offset, 1 + offset,
            1 + offset, 0 + offset, 4 + offset,
            // top       
            3 + offset, 2 + offset, 6 + offset,
            6 + offset, 7 + offset, 3 + offset
        });

    number_of_bount_tex++;
}

void DrawWindow::Render(std::vector<GLuint> texture_ids, GLuint pallete_id, GLuint Program_id_fbo, GLuint Program_id_window) //number of object to be rendered to the scene
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // TODO: add a static drawing and dynamic drawing to renderer where the geometry is rebuilt every frame for all entities.
    // also add animations by binding the textures at difrent times. Just let opengl handle all the loading of the objects in and out vram.
    
    // Render to the frame buffer:
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST); // use the depth test for this buffer

    // Clear the screen color \/     depth \/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(Program_id_fbo); // use the main voxel shader to render to the frame buffer

    glBindVertexArray(vao); // Bind the VAO for the framebuffer;


    // Draws all of current textures stored in texture_ids in order to the coresponding geometry
    int tex_size = texture_ids.size();
    int max_slots = gl_max_tex_slots_for_fbo;
    for (int i = 0; i <= (tex_size / gl_max_tex_slots_for_fbo); i++)
    {
        // Calculate the total number of texture slots to be used this frame
        int total = i * gl_max_tex_slots_for_fbo;
        if (tex_size - total < gl_max_tex_slots_for_fbo)
        {
            max_slots = tex_size - total;
        }
    
        for (int j = 0; j < max_slots; j++)
        {
           glActiveTexture(GL_TEXTURE0 + j);
           glBindTexture(GL_TEXTURE_3D, texture_ids[j + total]);
        }

        // Find the current batch pointer refrence:
        // first outputs 0 then last batch size then 2nd to last batch size + last batch size .ect
        int geom_ptr_offset = 0;
        for (int k = 0; k < i; k++) {
            geom_ptr_offset += geometry_batch_size.at(k);
        }

        glDrawElements(GL_TRIANGLES, 36 * geometry_batch_size.at(i), GL_UNSIGNED_INT, (GLvoid*)(4 * 36 * geom_ptr_offset));
    }

    // TODO: Check if the original frame buffer slot is 0 for glBINDFRAMEBUFFER:
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // unbind the frame buffer
    glDisable(GL_DEPTH_TEST); // disable the depth test for this buffer
    glClear(GL_COLOR_BUFFER_BIT); // clear the screen
    glUseProgram(Program_id_window); // use the window program
    glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);

    glBindVertexArray(vao_win);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Use main FBO for everything else
    glUseProgram(Program_id_fbo);
}

void DrawWindow::ResizeWindowCallback(GLFWwindow* window, int width, int height)
{
    
}

void DrawWindow::Destroy()
{
    glDeleteVertexArrays(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &vbo_win);
    glDeleteVertexArrays(1, &vao_win);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &fbo);
    glDeleteBuffers(1, &rbo);
    glDeleteTextures(1, &TextureColorBuffer);
}

DrawWindow::~DrawWindow()
{
  
}