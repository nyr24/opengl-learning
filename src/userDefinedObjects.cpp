#include "animation.hpp"
#include "geometryObject.hpp"
#include "matrix.hpp"
#include "renderer.hpp"
#include "vec.hpp"
#include <concepts>
#include <vector>

namespace my_gl {
    namespace math {
        template<std::floating_point T>
        class Matrix44;
    }

    // CubeCreature
    constexpr int cube_creature_vert_count{ 36 };

    GeometryObjectComplex create_cube_creature(const Program& program, const VertexArray& vertex_array) {
        return std::vector<GeometryObjectPrimitive>{
            // head
            GeometryObjectPrimitive{
                std::vector<TransformsByType>{
                    {
                        math::TransformationType::TRANSLATION,
                        {
                            math::Transformation<float>::translation({0.0f, 1.3f, 0.0f})
                        },
                        {}
                    },
                    {
                        math::TransformationType::ROTATION,
                        {
                            math::Transformation<float>::rotation(180.0f, math::Global::AXIS::Y)
                        },
                        {}
                    },
                    {
                        math::TransformationType::SCALING,
                        {
                            math::Transformation<float>::scaling({0.8f, 0.5f, 0.4f})
                        },
                        {}
                    },
                },
                cube_creature_vert_count,
                0,
                program,
                vertex_array,
                GL_TRIANGLES,
                {}
            },
            // body
            GeometryObjectPrimitive{
                std::vector<TransformsByType>{
                    {
                        math::TransformationType::ROTATION,
                        {
                            math::Transformation<float>::rotation(270.0f, math::Global::AXIS::Y)
                        },
                        {}
                    },
                    {
                        math::TransformationType::SCALING,
                        {
                            math::Transformation<float>::scaling({1.0f, 2.25f, 1.0f})
                        },
                        {}
                    },
                },
                cube_creature_vert_count,
                0,
                program,
                vertex_array,
                GL_TRIANGLES,
                {}
            },
            // left arm
            GeometryObjectPrimitive{
                std::vector<TransformsByType>{
                    {
                        math::TransformationType::TRANSLATION,
                        {
                            math::Transformation<float>::translation({-0.8f, 0.3f, 0.0f})
                        },
                        {}
                    },
                    {
                        math::TransformationType::ROTATION,
                        {
                            math::Transformation<float>::rotation(-55.0f, math::Global::AXIS::Z)
                        },
                        {}
                    },
                    {
                        math::TransformationType::SCALING,
                        {
                            math::Transformation<float>::scaling({0.3f, 1.5f, 0.3f})
                        },
                        {}
                    },
                },
                cube_creature_vert_count,
                0,
                program,
                vertex_array,
                GL_TRIANGLES,
                {}
            },
            // right arm
            GeometryObjectPrimitive{
                std::vector<TransformsByType>{
                    {
                        math::TransformationType::TRANSLATION,
                        {
                            math::Transformation<float>::translation({0.8f, 0.3f, 0.0f})
                        },
                        {}
                    },
                    {
                        math::TransformationType::ROTATION3d,
                        {
                            math::Transformation<float>::rotation3d({0.0f, 0.0f, 55.0f})
                        },
                        {
                            Animation<float>::rotation3d(
                                3.0f,
                                5.0f,
                                { 0.0f, 0.0f, 0.0f },
                                { -90.0f, -45.0f, 0.0f },
                                Bezier_curve_type::EASE_IN_OUT,
                                Loop_type::INVERT
                            )
                        }
                    },
                    {
                        math::TransformationType::SCALING,
                        {
                            math::Transformation<float>::scaling({0.3f, 1.5f, 0.3f})
                        },
                        {}
                    },
                },
                cube_creature_vert_count,
                0,
                program,
                vertex_array,
                GL_TRIANGLES,
                {}
            },
            // left foot
            GeometryObjectPrimitive{
                std::vector<TransformsByType>{
                    {
                        math::TransformationType::TRANSLATION,
                        {
                            math::Transformation<float>::translation({-0.25f, -1.5f, 0.0f})
                        },
                        {}
                    },
                    {
                        math::TransformationType::ROTATION,
                        {
                            math::Transformation<float>::rotation(-20.0f, math::Global::AXIS::Z)
                        },
                        {}
                    },
                    {
                        math::TransformationType::SCALING,
                        {
                            math::Transformation<float>::scaling({0.5f, 2.7f, 0.5f})
                        },
                        {}
                    },
                },
                cube_creature_vert_count,
                0,
                program,
                vertex_array,
                GL_TRIANGLES,
                {}
            },
            // right foot
            GeometryObjectPrimitive{
                std::vector<TransformsByType>{
                    {
                        math::TransformationType::TRANSLATION,
                        {
                            math::Transformation<float>::translation({0.25f, -1.5f, 0.0f})
                        },
                        {}
                    },
                    {
                        math::TransformationType::ROTATION,
                        {
                            math::Transformation<float>::rotation(20.0f, math::Global::AXIS::Z)
                        },
                        {}
                    },
                    {
                        math::TransformationType::SCALING,
                        {
                            math::Transformation<float>::scaling({0.5f, 2.7f, 0.5f})
                        },
                        {}
                    },
                },
                cube_creature_vert_count,
                0,
                program,
                vertex_array,
                GL_TRIANGLES,
                {}
            }
        };
    }
}
