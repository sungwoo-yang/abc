#version 300 es

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec2 a_Position;

out vec2 v_TexCoord;

uniform mat3 u_ModelMatrix;

uniform Camera
{
    mat3 u_ViewProjectionMatrix;
} u_Camera;

void main()
{
    vec3 position_3d = vec3(a_Position, 0.0);

    gl_Position = vec4(u_Camera.u_ViewProjectionMatrix * u_ModelMatrix * position_3d, 1.0);
    
    v_TexCoord = a_Position;
}