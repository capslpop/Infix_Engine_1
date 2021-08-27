#include "prerequisites.h"

std::string get_current_dir() // gets current directory only for windows as of now
{ 
    char buff[FILENAME_MAX]; //create string buffer to hold path
    _getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    
    for (int i = 0; i < current_working_dir.length(); ++i) { // itterates through all known '\' and replaces it with '/'
        if (current_working_dir[i] == '\\') // [\ = '\\']
            current_working_dir[i] = '/';
    }

    return current_working_dir;
}

int main(int, char**)
{
    //std::string current_dir = get_current_dir();
    //printf("Current Directory: %s\n", current_dir.c_str());
    DrawWindow drawwindow;  // Create/Bind posiotns and indacies for quad
    GLFWwindow* window = drawwindow.InitWindow();

    GLuint programid = Shaders::CompileShaders();

    TextureLoader3D wut;
    std::string three_d_tex_file_path = "/Assets/VoxelObjects/building2.xraw";
    wut.CreateTexture((current_dir + three_d_tex_file_path).c_str(), programid, "nature");

    
    drawwindow.InitCube(wut.texture_data.width, wut.texture_data.height, wut.texture_data.depth);


    // add uniform varables
    GLint camera_mat4 = glGetUniformLocation(programid, "camera_mat4");
    GLint cam_pos = glGetUniformLocation(programid, "cam_pos");
    GLint slider_var = glGetUniformLocation(programid, "slider_var");
    GLint resolution = glGetUniformLocation(programid, "resolution"); // Not in use as of 8/19/21
    

    DebugGui debuggui;
    ImGuiIO& io = debuggui.InitDebugGui(window);

    InputDevices inputdevices(window);
    inputdevices.AddKeyboard(key_callback);
    inputdevices.AddMouse(cursor_position_callback, mouse_button_callback);

    Camera camera;

    TextureLoader2D pebles;
    std::string two_d_tex_file_path = "/Assets/VoxelObjects/2d.xraw";
    pebles.CreateTextureXRAW((current_dir + two_d_tex_file_path).c_str(), programid, "Pebles");
    

    FileHandeler file_handeler(window, programid);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Update the current time between each frame:
        camera.update_delta_time();

        inputdevices.Update();


        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glm::mat4 CamMat4 = camera.UpdateCameraMat(display_w, display_h, inputdevices.mouse_dx, inputdevices.mouse_dy);

        glUniformMatrix4fv(camera_mat4, 1, GL_FALSE, glm::value_ptr(CamMat4));
        glUniform3f(cam_pos, camera.cam_pos.x, camera.cam_pos.y, camera.cam_pos.z);
        glUniform2f(resolution, display_w, display_h);

        debuggui.NewFrame();
        if (debuggui.SimpleWindow(slider_var))
        {
            Shaders::DestroyShader(programid);
            programid = Shaders::CompileShaders();

            
            TextureLoader3D wut; // rebind the texture to the new shader ID
            std::string three_d_tex_file_path = "/Assets/VoxelObjects/building2.xraw";
            wut.CreateTexture((current_dir + three_d_tex_file_path).c_str(), programid, "nature");

        }

        debuggui.AnotherWindow();
        debuggui.CustomWindow();
        debuggui.Render();

        // Clear the screen color \/     depth \/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the Quad
        drawwindow.Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    debuggui.Destroy();
    Shaders::DestroyShader(programid);
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void InputDevices::KeyDown(int key)
{
    switch (key)
    {
    case GLFW_KEY_W:
        input_move_camera_forward  = -1.0;
        break;
    case GLFW_KEY_S:
        input_move_camera_forward  = 1.0;
        break;
    case GLFW_KEY_A:
        input_move_camera_sideways = -1.0;
        break;
    case GLFW_KEY_D:
        input_move_camera_sideways = 1.0;
        break;
    
    case GLFW_KEY_SPACE:
        input_move_camera_upward = -1.0;
        break;
    case GLFW_KEY_LEFT_SHIFT:
        input_move_camera_upward = 1.0;
        break;


    default:
        break;
    }
}

void InputDevices::KeyUp(int key)
{
    switch (key)
    {
    case GLFW_KEY_W:
        input_move_camera_forward = 0.0;
        break;
    case GLFW_KEY_S:
        input_move_camera_forward = 0.0;
        break;
    case GLFW_KEY_A:
        input_move_camera_sideways = 0.0;
        break;
    case GLFW_KEY_D:
        input_move_camera_sideways = 0.0;
        break;

    case GLFW_KEY_SPACE:
        input_move_camera_upward = 0.0;
        break;
    case GLFW_KEY_LEFT_SHIFT:
        input_move_camera_upward = 0.0;
        break;


    default:
        break;
    }
}

void InputDevices::MousePos(double x, double y)
{

}

void InputDevices::MouseButtonDown(int button)
{
    //printf("you pressed key num: %i \n", button);
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_2:
        break;


    default:
        break;
    }
}

void InputDevices::MouseButtonUp(int button)
{
    //printf("you released\n");
}