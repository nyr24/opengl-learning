#version 330

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_mvp;

flat    out vec3 passed_color;
smooth  out vec3 passed_normal;
smooth  out vec3 passed_frag_pos;

void main() {
    vec4 a_pos_homogen      =   vec4(a_pos, 1.0);
    gl_Position             =   u_mvp * a_pos_homogen;
    passed_frag_pos         =   vec3(u_model * a_pos_homogen);
    passed_color            =   a_color;
    passed_normal           =   a_normal;
}
