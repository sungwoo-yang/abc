#version 300 es
precision mediump float; // Set default precision for floats
precision mediump sampler2D; // Set default precision for samplers

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

in vec2 v_uv; // Received texture coordinate from the vertex shader (already transformed)

uniform sampler2D u_texture; // The texture sampler
uniform vec4 u_tint_color;   // The tint color passed from the C++ application

layout(location = 0) out vec4 frag_color; // Output color for this fragment

void main()
{
    // 1. Sample the color from the texture at the received texture coordinate (v_uv).
    vec4 tex_color = texture(u_texture, v_uv);

    // 2. Multiply the sampled texture color by the tint color.
    //    This allows applying effects like coloring, fading, etc.
    frag_color = tex_color * u_tint_color;

    // 3. Discard (make transparent) fragments with very low alpha values.
    //    This prevents nearly invisible pixels from affecting blending or causing artifacts.
    if (frag_color.a < 0.01)
    {
        discard; // Tell OpenGL to not draw this fragment
    }
}