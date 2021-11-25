cmake_minimum_required (VERSION 3.8)

add_executable (Infix_Engine 
"main.cpp"
"prerequisites.h" 
"includes/IMGUI/imconfig.h" 
"includes/IMGUI/imgui.cpp" 
"includes/IMGUI/imgui.h" 
"includes/IMGUI/imgui_demo.cpp" 
"includes/IMGUI/imgui_draw.cpp" 
"includes/IMGUI/imgui_internal.h" 
"includes/IMGUI/imgui_tables.cpp" 
"includes/IMGUI/imgui_widgets.cpp" 
"includes/IMGUI/imstb_rectpack.h"
"includes/IMGUI/imstb_textedit.h" 
"includes/IMGUI/imstb_truetype.h" 
"includes/IMGUI/imgui_impl_opengl3.cpp" 
"includes/IMGUI/imgui_impl_opengl3.h" 
"includes/IMGUI/imgui_impl_glfw.cpp"
"includes/IMGUI/imgui_impl_glfw.h" 
"3D-Graphics/Shaders/Shaders.h" 
"3D-Graphics/DrawWindow/DrawWindow.h"
"3D-Graphics/Camera/Camera.h" 
 "includes/IMAGE_LOADER/stb_image.h"
 "Assets/AssetsHandler/TextureHandeler/TextureLoader2D.h" 
 "Assets/AssetsHandler/TextureHandeler/TextureLoader3D.h" 
 "Assets/AssetsHandler/TextureHandeler/TextureLoader1D.h" "VoxelPhysics/VoxelPhysics.h")

add_subdirectory(includes/glad/)
add_subdirectory(includes/glfw/)
add_subdirectory(includes/glm/)

target_link_libraries(Infix_Engine PUBLIC glad glfw glm)