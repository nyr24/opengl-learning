#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "utils.hpp"

std::unique_ptr<my_gl::Window> my_gl::init() {
    std::cout << "Starting GLFW context, OpenGL 3.3\n";

    my_gl::initGLFW();

    auto window_ptr{ 
        std::make_unique<my_gl::Window>( 700, 500, "my_window", nullptr, nullptr) 
    };

    my_gl::initGLEW();

    // error handling
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(my_gl::GLDebugMessageCallback, NULL);

    return window_ptr;
}

void my_gl::initGLFW() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

void my_gl::initGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        std::exit(EXIT_FAILURE);
    } 
}

void my_gl::initVertexBuffer(GLuint* bufferObjectId, const float vertexPositions[], size_t vertexCount, GLenum drawMode) {
    glCreateBuffers(1, bufferObjectId);

    glBindBuffer(GL_ARRAY_BUFFER, *bufferObjectId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount, vertexPositions, drawMode);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void my_gl::initVertexArrayObject(GLuint* vaoId) {
    glGenVertexArrays(1, vaoId);
    glBindVertexArray(*vaoId);
}


GLuint my_gl::createProgram(const char* vertexShaderFilePath, const char* fragmentShaderFilePath) {
    GLuint program{ glCreateProgram() };

    GLuint vertexShader{ my_gl::createShader(GL_VERTEX_SHADER, vertexShaderFilePath) };
    GLuint fragShader{ my_gl::createShader(GL_FRAGMENT_SHADER, fragmentShaderFilePath) };

    std::vector<GLuint> shaders{ vertexShader, fragShader };
    
    for (int i{ 0 }; i < shaders.size(); ++i) {
        glAttachShader(program, shaders[i]);
    }

    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::string buffer;
        buffer.reserve(infoLogLength + 1);
        glGetProgramInfoLog(program, infoLogLength, nullptr, buffer.data());

        std::cout << "Error when linking a program:\n" << buffer << '\n';
    }

    for (const auto shaderId : shaders) {
        glDetachShader(program, shaderId);
        glDeleteShader(shaderId);
    }

    return program;
}


GLuint my_gl::createShader(GLenum shaderType, const char* filePath) {        
    std::ifstream shaderFile{ filePath, std::ios_base::binary };

    if (!shaderFile.is_open()) {
        std::cerr << "shader source load error from " << filePath << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::stringstream shaderSourceStream;
    shaderSourceStream << shaderFile.rdbuf();
    shaderFile.close();
    std::string shaderSourceStr{ shaderSourceStream.str()};
    const GLchar* cStringShaderSource{ static_cast<const GLchar*>(shaderSourceStr.c_str())};

    GLuint shaderId{ glCreateShader(shaderType) };
    glShaderSource(shaderId, 1, &cStringShaderSource, nullptr);
    glCompileShader(shaderId);

    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        char infoLogBuffer[infoLogLength + 1];
        glGetShaderInfoLog(shaderId, infoLogLength, nullptr, infoLogBuffer);

        std::string_view shaderTypeStr;

        switch (shaderType) {
        case GL_VERTEX_SHADER:
            shaderTypeStr = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            shaderTypeStr = "fragment";
            break;
        case GL_GEOMETRY_SHADER:
            shaderTypeStr = "geometry";
            break;
        }

        std::cout << "Error when compiling a shader:\n" << shaderTypeStr << '\n' 
        << infoLogBuffer << '\n';
    }

    return shaderId;
}


void my_gl::GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) {
    const char* _source;
    const char* _type;
    const char* _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    printf("[Open gl error]%d: %s of %s severity, raised from %s: %s\n",
        id, _type, _severity, _source, msg);
} 