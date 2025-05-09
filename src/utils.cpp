#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>
#include "utils.hpp"
#include "globals.hpp"
#include "camera.hpp"
#include <STB_IMG/stb_image.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

my_gl::Window my_gl::init_window() {
    std::cout << "Starting GLFW context, OpenGL 3.3\n";

    my_gl::init_GLFW();

    my_gl::Window window{ globals::window_props.width, globals::window_props.height, "nyr_window", nullptr, nullptr };

    my_gl::init_GLEW();

    // error handling
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(my_gl::callback_debug_message, NULL);

    // culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);

    // user input && callbacks
    glfwSetFramebufferSizeCallback(window.ptr_raw(), my_gl::callback_framebuffer_size);
    glfwSetKeyCallback(window.ptr_raw(), my_gl::callback_keyboard);
    glfwSetCursorPosCallback(window.ptr_raw(), my_gl::callback_mouse_move);
    glfwSetScrollCallback(window.ptr_raw(), my_gl::callback_scroll);
    glfwSetInputMode(window.ptr_raw(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // points drawing
    glEnable(GL_PROGRAM_POINT_SIZE);

    // other
    stbi_set_flip_vertically_on_load(true);
 
    return window;
}

void my_gl::init_GLFW() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        std::exit(EXIT_FAILURE);
    }
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
}

void my_gl::init_GLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

GLuint my_gl::create_program(const char* vertexShaderFilePath, const char* fragmentShaderFilePath) {
    GLuint program{ glCreateProgram() };

    GLuint vertexShader{ my_gl::create_shader(GL_VERTEX_SHADER, vertexShaderFilePath) };
    GLuint fragShader{ my_gl::create_shader(GL_FRAGMENT_SHADER, fragmentShaderFilePath) };

    std::vector<GLuint> shaders{ vertexShader, fragShader };
 
    for (size_t i{ 0 }; i < shaders.size(); ++i) {
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


GLuint my_gl::create_shader(GLenum shaderType, const char* filePath) {        
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
 
        auto infoLogBuffer = std::make_unique_for_overwrite<char[]>(infoLogLength + 1);
        glGetShaderInfoLog(shaderId, infoLogLength, nullptr, infoLogBuffer.get());

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

// callbacks
void my_gl::callback_framebuffer_size(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void my_gl::callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GL_TRUE);
        break;
    case GLFW_KEY_W:
        my_gl::globals::camera.process_keyboard_input(my_gl::Camera_movement::FORWARD);
        break;
    case GLFW_KEY_S:
        my_gl::globals::camera.process_keyboard_input(my_gl::Camera_movement::BACKWARD);
        break;
    case GLFW_KEY_A:
        my_gl::globals::camera.process_keyboard_input(my_gl::Camera_movement::LEFT);
        break;
    case GLFW_KEY_D:
        my_gl::globals::camera.process_keyboard_input(my_gl::Camera_movement::RIGHT);
        break;
    }
}

void my_gl::callback_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    my_gl::globals::camera.process_mouse_input(
        static_cast<float>(xpos), static_cast<float>(ypos)
    );
}

void my_gl::callback_scroll(GLFWwindow* window, double xoffset, double yoffset) {
    my_gl::globals::camera.process_scroll_input(
        static_cast<float>(xoffset), static_cast<float>(yoffset)
    );
}

void my_gl::callback_debug_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) {
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

    printf("[Open gl note]%d: %s of %s severity, raised from %s: %s\n",
        id, _type, _severity, _source, msg);
}

void my_gl::print_max_vert_attrs_supported() {
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
}
