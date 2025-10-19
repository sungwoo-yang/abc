#version 300 es
precision mediump float;

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

in vec2 v_TexCoord;

out vec4 o_FragColor;

uniform vec4  u_FillColor;
uniform vec4  u_LineColor;
uniform float u_LineWidth;
uniform int   u_ShapeType;
uniform vec2  u_WorldSize;
uniform vec2  u_QuadSize;

float sd_Circle(vec2 p, float r)
{
    return length(p) - r;
}

float sd_Rectangle(vec2 p, vec2 b)
{
    vec2 d = abs(p) - b;
    return length(max(d, 0.0));
}


void main()
{
    vec2 uv_ratio = u_WorldSize / u_QuadSize;

    if (uv_ratio.x == 0.0) uv_ratio.x = 0.0001;
    if (uv_ratio.y == 0.0) uv_ratio.y = 0.0001;
    
    vec2 shape_uv = v_TexCoord / uv_ratio;

    float dist_circle = sd_Circle(shape_uv, 0.5);
    float dist_rect = sd_Rectangle(shape_uv, vec2(0.5));
    
    float d = mix(dist_circle, dist_rect, float(u_ShapeType));
    
    float line_width_uv = u_LineWidth / max(u_WorldSize.x, u_WorldSize.y);

    float aa = fwidth(d);

    float fill_alpha = smoothstep(aa, -aa, d);

    float line_alpha = smoothstep(line_width_uv + aa, line_width_uv - aa, d);

    vec4 color = mix(u_LineColor, u_FillColor, fill_alpha);

    color.a *= line_alpha;
    
    o_FragColor = color;
}
