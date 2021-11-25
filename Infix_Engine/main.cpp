#include "prerequisites.h"

int main(int, char**)
{
    printlogo();

    DrawWindow drawwindow;
    GLFWwindow* window = drawwindow.InitWindow();

    GLuint program_id_window = Shaders::CompileShadersWindow(); // This needs to go first because we don't need to use the program untill rendering
    GLuint program_id_fbo = Shaders::CompileShadersFBO(); // We need to put this shader last because it is what we are sending all our textures to.
   
    drawwindow.AddTextureSlots(program_id_fbo);

    int bound_texture_count = 0;
    TextureLoader3D voxel_object_one; //Church_Of_St_Sophia //Game_test_terrain
    std::vector<GLuint> texture_ids = voxel_object_one.CreateTextureVOX("./Voxel_Data/Church_Of_St_Sophia.vox", bound_texture_count, drawwindow, program_id_fbo);

    printf("number of textures bound: %\i ", bound_texture_count);

    // Generate sampler for frame buffer that is supplied to window
    glUniform1i(glGetUniformLocation(program_id_window, "u_Textures"), 0);

    drawwindow.BindGeometry();

    // add uniform varables
    GLint camera_mat4 = glGetUniformLocation(program_id_fbo, "camera_mat4");
    GLint cam_pos = glGetUniformLocation(program_id_fbo, "cam_pos");
    
    DebugGui debuggui;
    ImGuiIO& io = debuggui.InitDebugGui(window);

    InputDevices inputdevices(window);
    inputdevices.AddKeyboard(key_callback);
    inputdevices.AddMouse(cursor_position_callback, mouse_button_callback);

    Camera camera;

    FileHandeler file_handeler(window, program_id_fbo);

    printf("this is the uniform pallete: %u \n", voxel_object_one.pallete_id);

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

        debuggui.NewFrame();
        debuggui.SimpleWindow(voxel_object_one.palette_IMGUI);
        debuggui.AnotherWindow();
        debuggui.CustomWindow();
        debuggui.Render();
        
        // TODO: update Physics engine before rendering

        // Draw all objects to the screen
        drawwindow.Render(texture_ids, voxel_object_one.pallete_id, program_id_fbo, program_id_window);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    debuggui.Destroy();
    Shaders::DestroyShader(program_id_fbo);

    // Destroy all textures:
    glDeleteTextures(texture_ids.size(), &texture_ids[0]);

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