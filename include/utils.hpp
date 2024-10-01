#pragma once
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <string_view>
#include "window.hpp"


namespace my_gl {
    void init(my_gl::Window* window_ptr_out);

    void initGLFW();
    void initGLEW();

    GLuint createShader(GLenum shaderType, const char* filePath);
    GLuint createProgram(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);
    
    void initVertexBuffer(GLuint* bufferObjectId, const float vertexPositions[], size_t vertexCount, GLenum drawMode);
    void initVertexArrayObject(GLuint* vaoId);

    void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data);
}