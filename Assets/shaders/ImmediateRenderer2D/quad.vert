#version 300 es

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout (location = 0) in vec2 a_position; // Vertex position (local space, typically -0.5 to 0.5)
layout (location = 1) in vec2 a_tex_coord; // Texture coordinate (typically 0.0 to 1.0)

// Uniforms provided by the C++ application
uniform mat3 u_ndc_matrix;   // Matrix to convert from world space to Normalized Device Coordinates (NDC)
uniform mat3 u_model_matrix; // Matrix to transform the quad from local space to world space (includes translation, rotation, scale)
uniform mat3 u_uv_matrix;    // Matrix to transform the input texture coordinates (for sprite sheets/atlases)

out vec2 v_uv; // Pass the transformed texture coordinate to the fragment shader

void main()
{
    // 1. Transform the input texture coordinate using the uv_matrix.
    //    We use a vec3 because matrix multiplication requires matching dimensions.
    //    The .xy extracts the resulting 2D coordinate.
    v_uv = (u_uv_matrix * vec3(a_tex_coord, 1.0)).xy;

    // 2. Transform the vertex position:
    //    a) Convert local position (a_position) to a vec3 (adding z=1.0 for matrix multiplication).
    //    b) Apply the model matrix to transform from local space to world space.
    //    c) Apply the NDC matrix to transform from world space to NDC space.
    vec3 ndc_pos = u_ndc_matrix * u_model_matrix * vec3(a_position, 1.0);

    // 3. Set the final vertex position for OpenGL.
    //    gl_Position requires a vec4. We use the calculated ndc_pos.xy for x and y,
    //    0.0 for z (since we're in 2D), and 1.0 for w (standard for positions).
    gl_Position = vec4(ndc_pos.xy, 0.0, 1.0);
}