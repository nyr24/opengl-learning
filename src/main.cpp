#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <ctime>
#include "animation.hpp"
#include "math.hpp"
#include "matrix.hpp"
#include "sharedTypes.hpp"
#include "vec.hpp"
#include "utils.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "geometryObject.hpp"
#include "texture.hpp"
#include "globals.hpp"
#include "camera.hpp"
#include "meshes.hpp"
#include "userDefinedObjects.hpp"

int main() {
    my_gl::Window window{ my_gl::init_window() };

    constexpr uint16_t texture_offset{ sizeof(float) * 3 * 4 * 6 };
    constexpr uint16_t color_offset{ texture_offset + sizeof(float) * 2 * 4 * 6 };
    constexpr uint16_t normal_offset{ color_offset + sizeof(float) * 3 * 4 * 6 };

    my_gl::Program world_shader{
        "shaders/vertShader.glsl",
        "shaders/fragShader.glsl",
        {
            { .name = "a_pos", .gl_type = GL_FLOAT, .count = 3, .byte_stride = 0, .byte_offset = 0 },
            { .name = "a_color", .gl_type = GL_FLOAT, .count = 3, .byte_stride = 0, .byte_offset = color_offset },
            { .name = "a_normal", .gl_type = GL_FLOAT, .count = 3, .byte_stride = 0, .byte_offset = normal_offset },
        },
        {
            { .name = "u_mvp_mat" },
            { .name = "u_model_view_mat" },
            { .name = "u_normal_mat" },
            { .name = "u_light_color" },
            { .name = "u_light_pos" },
            { .name = "u_view_pos" },
        }
    };

    my_gl::Program light_shader{
        "shaders/vertShaderLight.glsl",
        "shaders/fragShaderLight.glsl",
        {
            { .name = "a_pos", .gl_type = GL_FLOAT, .count = 3, .byte_stride = 0, .byte_offset = 0 },
        },
        {
            { .name = "u_mvp_mat" },
            { .name = "u_color" }
        }
    };

// move this to object to dynamically assign uniform value,
//this would be overwritten if specified more textures than uniforms
    //std::vector<my_gl::Texture> textures = {*/
    //    { "res/mine_red.jpg", program2, program2.get_uniform("u_tex_data1"), 0, GL_TEXTURE0 },
    //    { "res/mine_green.jpg", program2, program2.get_uniform("u_tex_data2"), 1, GL_TEXTURE1 },
    //};

    my_gl::VertexArray vertex_arr_world{
        my_gl::meshes::cube_mesh,
        world_shader
    };

    my_gl::VertexArray vertex_arr_light{
        my_gl::meshes::cube_mesh,
        light_shader
    };

    // transformations
    std::vector<my_gl::TransformsByType> world_transforms = {
        {
            my_gl::math::TransformationType::TRANSLATION,
            std::vector{
                my_gl::math::Transformation<float>::scaling({0.85f, 0.85f, 0.85f})
            },
            {}
        },
        {
            my_gl::math::TransformationType::ROTATION,
            {},
            {
                my_gl::Animation<float>::rotation_single_axis(
                    5.0f,
                    0.0f,
                    0.0f,
                    360.0f,
                    my_gl::math::Global::AXIS::X,
                    my_gl::Bezier_curve_type::EASE_IN_OUT,
                    my_gl::Loop_type::INVERT
                )
            }
        }
    };

    std::vector<my_gl::TransformsByType> light_transforms = {
        {
            my_gl::math::TransformationType::TRANSLATION,
            {
                my_gl::math::Transformation<float>::translation(my_gl::globals::light_pos),
                my_gl::math::Transformation<float>::scaling({0.4f, 0.2f, 0.2f}),
            },
            // {
            //     my_gl::Animation<float>::translation(
            //         10.0f,
            //         0.0f,
            //         my_gl::math::Vec3<float>{ my_gl::globals::light_pos },
            //         my_gl::math::Vec3<float>{ my_gl::globals::light_pos[0] - 10.0f, my_gl::globals::light_pos[1], my_gl::globals::light_pos[2] },
            //         my_gl::Bezier_curve_type::LINEAR,
            //         my_gl::Loop_type::INVERT
            //     )
            // },
            {}
        }
    };

    // primitives
    std::vector<my_gl::GeometryObjectPrimitive> primitives = {
        // world cube
        {
            std::move(world_transforms),
            36,
            0,
            world_shader,
            vertex_arr_world,
            GL_TRIANGLES,
            {}
        },
        // light
        {
            std::move(light_transforms),
            36,
            0,
            light_shader,
            vertex_arr_light,
            GL_TRIANGLES,
            {}
        }
    };

    // camera
    auto view_mat{ my_gl::globals::camera.get_view_mat() };
    auto proj_mat{ my_gl::math::Matrix44<float>::perspective_fov(
        my_gl::globals::camera.fov, my_gl::globals::camera.aspect, 0.1f, 50.0f
    )};

    my_gl::Renderer renderer{
        std::vector<my_gl::GeometryObjectComplex>{}, //my_gl::create_cube_creature(world_shader, vertex_arr_world) },
        std::move(primitives),
        std::move(view_mat),
        std::move(proj_mat),
    };

    world_shader.set_uniform_value("u_light_color", 1.0f, 1.0f, 1.0f);

    my_gl::math::Vec3<float> light_pos_view_coords{ renderer._view_mat * my_gl::globals::light_pos };

    world_shader.set_uniform_value("u_light_pos",
        // view coords
        // light_pos_view_coords[0],
        // light_pos_view_coords[1],
        // light_pos_view_coords[2]
        // world coords
        my_gl::globals::light_pos[0],
        my_gl::globals::light_pos[1],
        my_gl::globals::light_pos[2]
    );
    world_shader.set_uniform_value("u_view_pos",
        // view coords
        // 0.0f, 0.0f, 0.0f
        // world coords
        my_gl::globals::camera.camera_pos[0],
        my_gl::globals::camera.camera_pos[1],
        my_gl::globals::camera.camera_pos[2]
    );
    light_shader.set_uniform_value("u_color", 1.0f, 1.0f, 1.0f);

    bool is_rendering_started{false};
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window.ptr_raw())) {
        auto start_frame{ std::chrono::steady_clock::now() };
        if (!is_rendering_started) {
            renderer._rendering_time_start = start_frame;
            is_rendering_started = true;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer._view_mat = my_gl::globals::camera.get_view_mat();
        renderer._proj_mat = my_gl::math::Matrix44<float>::perspective_fov(
            my_gl::globals::camera.fov, my_gl::globals::camera.aspect, 0.1f, 50.0f
        );

        my_gl::math::Vec3<float> light_pos_view_coords{ renderer._view_mat * my_gl::globals::light_pos };

        world_shader.set_uniform_value("u_light_pos",
            // view coords
            // light_pos_view_coords[0],
            // light_pos_view_coords[1],
            // light_pos_view_coords[2]
            // world coords
            my_gl::globals::light_pos[0],
            my_gl::globals::light_pos[1],
            my_gl::globals::light_pos[2]
        );
        world_shader.set_uniform_value("u_view_pos",
            // view coords:
            // 0.0f, 0.0f, 0.0f
            // world coords:
            my_gl::globals::camera.camera_pos[0],
            my_gl::globals::camera.camera_pos[1],
            my_gl::globals::camera.camera_pos[2]
        );

        float time_0to1 = my_gl::math::Global::map_duration_to01(renderer.get_curr_rendering_duration());
        renderer.render(time_0to1);

        glfwSwapBuffers(window.ptr_raw());
        glfwPollEvents();

        my_gl::Duration_sec frame_duration{ std::chrono::steady_clock::now() - start_frame };
        renderer.update_time(frame_duration);
        my_gl::globals::delta_time = frame_duration.count();
    }

    return 0;
}
