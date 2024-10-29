#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 u_mvp_mat;

flat out vec4 passed_color;

void main() {
    gl_Position = u_mvp_mat * vec4(position, 1.0);
    passed_color = color;
}