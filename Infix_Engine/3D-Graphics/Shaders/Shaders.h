#pragma once

class Shaders
{
public:
	Shaders();
    GLuint CreateShader(const int unsigned shader_type);
    GLuint LoadShaders(const char* file_path, GLint &Result, int &InfoLogLength);
	~Shaders();

    static GLuint CompileShaders();
    static void DestroyShader(GLuint programid);

private:

    GLuint ShaderID;
};

Shaders::Shaders()
{

}

GLuint Shaders::LoadShaders(const char* file_path, GLint &Result, int &InfoLogLength) {

    std::string ShaderCode;
    std::ifstream ShaderStream(file_path, std::ios::in);
    if (ShaderStream.is_open()) {  //to do add error handeling try catch
        std::stringstream sstr;
        sstr << ShaderStream.rdbuf();
        ShaderCode = sstr.str();
        ShaderStream.close();
    }
    else {
        printf("Impossible to open shader: %s\n", file_path);
        return 0;
    }

    // Compile Shader
    printf("Compiling shader : %s\n", file_path);
    char const* VertexSourcePointer = ShaderCode.c_str();
    glShaderSource(ShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(ShaderID);

    // Check Shader
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
        printf("%s\n", &ShaderErrorMessage[0]);
    }
}

GLuint Shaders::CreateShader(const int unsigned shader_type)
{
    ShaderID = glCreateShader(shader_type);
    return ShaderID;
}




Shaders::~Shaders()
{
}

GLuint Shaders::CompileShaders()
{
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Create and compile our GLSL program from the shaders
    Shaders vertexShader;
    GLuint vertexID = vertexShader.CreateShader(GL_VERTEX_SHADER);
    std::string vertex_file_loc = "/3D-Graphics/Shaders/Vertex.glsl";
    GLuint vertex_program = vertexShader.LoadShaders((current_dir + vertex_file_loc).c_str(),
        Result, InfoLogLength);

    Shaders fragmentShader;
    GLuint fragmentID = fragmentShader.CreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_file_loc = "/3D-Graphics/Shaders/Fragment.glsl";
    GLuint fragment_program = fragmentShader.LoadShaders((current_dir + fragment_file_loc).c_str(),
        Result, InfoLogLength);

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vertexID);
    glAttachShader(ProgramID, fragmentID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, vertexID);
    glDetachShader(ProgramID, fragmentID);

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    glUseProgram(ProgramID);

    return ProgramID;
}

void Shaders::DestroyShader(GLuint programid)
{
    glDeleteProgram(programid);
}
