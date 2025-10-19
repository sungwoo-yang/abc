#version 300 es

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout (location = 0) in vec2 a_position; // Vertex position (local space, -0.5 to 0.5)

// Uniforms from C++
uniform mat3 u_ndc_matrix;   // World space to NDC space matrix
uniform mat3 u_model_matrix; // Local space to World space matrix (for the rendering quad, potentially scaled up)
uniform vec2 u_world_size;   // Original size of the shape in world units (before scaling for line width)
uniform vec2 u_quad_size;    // Size of the rendering quad in world units (includes padding for line width)

out vec2 v_sdf_coord; // Coordinate used for SDF calculation in the fragment shader (-0.5 to 0.5 scaled by world/quad ratio)

void main()
{
    // Calculate the coordinate for SDF evaluation.
    // We scale the local vertex position (-0.5 to 0.5) by the ratio of the original shape size
    // to the actual quad size being rendered. This effectively maps the larger quad's corners
    // back to the [-0.5, 0.5] range relative to the original shape's dimensions, allowing the
    // fragment shader to calculate distances correctly within the original shape's space.
    // Division by zero is avoided assuming quad_size components are always > 0 if world_size components are > 0.
    // If quad_size could be zero, add checks/clamping.
     v_sdf_coord = a_position * u_world_size / u_quad_size;


    // Transform the vertex position to NDC space for rasterization.
    // 1. Convert local position to vec3 (add w=1.0).
    // 2. Apply model matrix (transforms the potentially larger quad to world space).
    // 3. Apply NDC matrix (transforms from world space to NDC).
    vec3 ndc_pos = u_ndc_matrix * u_model_matrix * vec3(a_position, 1.0);

    // Set the final vertex position for OpenGL (vec4 required).
    gl_Position = vec4(ndc_pos.xy, 0.0, 1.0);
}