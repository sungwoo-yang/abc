#version 300 es
precision mediump float;

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

in vec2 v_sdf_coord;

uniform int u_shape_type;
uniform vec4 u_fill_color;
uniform vec4 u_line_color;
uniform float u_line_width;
uniform highp vec2 u_world_size;

out vec4 frag_color;

float circle_sdf(vec2 p)
{
    return length(p) - 0.5;
}

float rectangle_sdf(vec2 p, vec2 half_size)
{
    vec2 d = abs(p) - half_size;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

void main()
{
    float dist;
    if (u_shape_type == 0)
    {
        dist = circle_sdf(v_sdf_coord);
    }
    else
    {
        dist = rectangle_sdf(v_sdf_coord, vec2(0.5));
    }

    float line_width_in_sdf = u_line_width / max(u_world_size.x, u_world_size.y);
    float inner_edge = -line_width_in_sdf;
    
    float inner_mix = smoothstep(inner_edge, inner_edge + fwidth(dist), dist);
    vec4 shape_color = mix(u_fill_color, u_line_color, inner_mix);

    float outer_mix = smoothstep(0.0, fwidth(dist), dist);
    frag_color = mix(shape_color, vec4(0.0), outer_mix);

    if (frag_color.a < 0.01)
    {
        discard;
    }
}