#pragma once

class DrawWindow
{
public:
	DrawWindow();
    GLFWwindow* InitWindow();
    void InitCube(float box_x, float box_y, float box_z);
    void Render();
    void Destroy();
	~DrawWindow();
private:
    unsigned int VBO, VAO, EBO;
};

DrawWindow::DrawWindow()
{
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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

    return window;
}

void DrawWindow::InitCube(float box_x, float box_y, float box_z)
{
    float x_offset = 0.0;
    float y_offset = 0.0;
    float z_offset = 300.0;

    float vertex_list[] =
    {
        // fist cube
        // front              //texture coordinates 
        -box_x, -box_y,  box_z,     0.0, 0.0, 1.0,
         box_x, -box_y,  box_z,     1.0, 0.0, 1.0,
         box_x,  box_y,  box_z,     1.0, 1.0, 1.0,
        -box_x,  box_y,  box_z,     0.0, 1.0, 1.0,
        // back                         
        -box_x, -box_y, -box_z,     0.0, 0.0, 0.0,
         box_x, -box_y, -box_z,     1.0, 0.0, 0.0,
         box_x,  box_y, -box_z,     1.0, 1.0, 0.0,
        -box_x,  box_y, -box_z,     0.0, 1.0, 0.0,

        // fist second
        // front              //texture coordinates 
        -box_x + x_offset, -box_y + y_offset,  box_z + z_offset,     0.0, 0.0, 1.0,
         box_x + x_offset, -box_y + y_offset,  box_z + z_offset,     1.0, 0.0, 1.0,
         box_x + x_offset,  box_y + y_offset,  box_z + z_offset,     1.0, 1.0, 1.0,
        -box_x + x_offset,  box_y + y_offset,  box_z + z_offset,     0.0, 1.0, 1.0,
        // back                         
        -box_x + x_offset, -box_y + y_offset, -box_z + z_offset,     0.0, 0.0, 0.0,
         box_x + x_offset, -box_y + y_offset, -box_z + z_offset,     1.0, 0.0, 0.0,
         box_x + x_offset,  box_y + y_offset, -box_z + z_offset,     1.0, 1.0, 0.0,
        -box_x + x_offset,  box_y + y_offset, -box_z + z_offset,     0.0, 1.0, 0.0,


    };

    unsigned int index_list[] =
    {
        //First cube
        // 
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3,

        //Second cube
        //
          // front
        0 + 8, 1 + 8, 2 + 8,
        2 + 8, 3 + 8, 0 + 8,
        // right
        1 + 8, 5 + 8, 6 + 8,
        6 + 8, 2 + 8, 1 + 8,
        // back
        7 + 8, 6 + 8, 5 + 8,
        5 + 8, 4 + 8, 7 + 8,
        // left
        4 + 8, 0 + 8, 3 + 8,
        3 + 8, 7 + 8, 4 + 8,
        // bottom
        4 + 8, 5 + 8, 1 + 8,
        1 + 8, 0 + 8, 4 + 8,
        // top
        3 + 8, 2 + 8, 6 + 8,
        6 + 8, 7 + 8, 3 + 8
    };


    // Create buffer and copy data
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_list), vertex_list, GL_STATIC_DRAW);

    // Create index buffer
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_list), index_list, GL_STATIC_DRAW);
    



    //**************************************************************************************************
    //              |Size of attributes|      |How many attributes|    |Offset from start of attributes|
    //Position attribute     |                      |                         |
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //***************************************************************************************************



    // Render only the front side of the cube!!
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    //glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
}

void Addgeometry(float x, float y, float z, float size_x, float size_y, float size_z)
{

}

void DrawWindow::Render()
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    
    glDrawElements(GL_TRIANGLES, 36*2, GL_UNSIGNED_INT, 0);
}

void DrawWindow::Destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

DrawWindow::~DrawWindow()
{
  
}