#pragma once



#include "includes/IMGUI/imgui.h"
#include "includes/IMGUI/imgui_impl_glfw.h"
#include "includes/IMGUI/imgui_impl_opengl3.h"

#include <glad/glad.h>  

// Include glfw3.h after our OpenGL definitions
#include "includes/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include <algorithm>
#include <ios>
#include <fstream>
//#include <filesystem>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <sstream>

#include <direct.h> // for windows file path

// GLM for Vector and Matrix Math:
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>  // Transformation related functions
#include <glm/gtc/type_ptr.hpp> 


// Image Loader
#define STB_IMAGE_IMPLEMENTATION
#include "includes/IMAGE_LOADER/stb_image.h"

const char* current_dir = "C:/Users/johnw/source/repos/Infix_Engine/Infix_Engine";

// Consol handeling
#include "GUI/output.h"

// Include files
#include "3D-Graphics/Shaders/Shaders.h"
#include "3D-Graphics/DrawWindow/DrawWindow.h"
#include "InputDevices/InputDevices.h"
#include "3D-Graphics/Camera/Camera.h"
#include "Assets/AssetsHandler/TextureHandeler/TextureLoader3D.h"
#include "Assets/AssetsHandler/TextureHandeler/TextureLoader2D.h"
#include "Assets/AssetsHandler/TextureHandeler/TextureLoader1D.h"
#include "Assets/AssetsHandler/FileHandeler.h"
#include "GUI/DebugGui.h"
//#include "VoxelPhysics/VoxelPhysics.h"