#pragma once

class DebugGui
{
public:
    DebugGui();
    ImGuiIO& InitDebugGui(GLFWwindow* window);
    void NewFrame();
    void DemoWindow();
    bool SimpleWindow(GLint slider_var);
    void AnotherWindow();
    void CustomWindow();
    void Render();
    void Destroy();
    ~DebugGui();

private:
    bool show_demo_window = true;
    bool show_another_window = false;
    bool show_custom_window = false;
    float slider = 0.0f;
    ImVec4 window_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

DebugGui::DebugGui()
{

}

ImGuiIO& DebugGui::InitDebugGui(GLFWwindow* window)
{
    // Poll and handle events (inputs, window resize, etc.)
       // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
       // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
       // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
       // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.


    const char* glsl_version = "#version 130";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return io;
}

void DebugGui::NewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugGui::DemoWindow()
{
    ImGui::ShowDemoWindow();
}

bool DebugGui::SimpleWindow(GLint slider_var)
{
    static int counter = 0;
    
    bool button = 0;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
    
    ImGui::Text("This is a window");               // Display some text (you can use a format strings too)
    //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);
    ImGui::Checkbox("Custom Window", &show_custom_window);
    
    ImGui::SliderFloat("float", &slider, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&window_color); // Edit 3 floats representing a color

    if (ImGui::Button("RECOMPILE SHADERS"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    {
        button = 1;
        counter++;
    }

    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    glUniform1f(slider_var, slider);

    return button;
}

void DebugGui::AnotherWindow()
{
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

void DebugGui::CustomWindow()
{
    if (show_custom_window)
    {
        ImGui::Begin("New Window", &show_custom_window);
        ImGui::Text("Where is this located?");
        ImGui::End();
    }
}

void DebugGui::Render()
{
    // Rendering
    ImGui::Render();
}

void DebugGui::Destroy()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

DebugGui::~DebugGui()
{

}