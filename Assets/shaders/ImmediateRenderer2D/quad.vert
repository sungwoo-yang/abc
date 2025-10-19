#version 300 es

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat3 u_model_matrix;

uniform Camera
{
    mat3 u_ndc_matrix;
} u_Camera;

void main()
{
    gl_Position = vec4(u_Camera.u_ndc_matrix * u_model_matrix * vec3(a_Position.xy, 1.0), 1.0);
    v_TexCoord = a_TexCoord;
}
