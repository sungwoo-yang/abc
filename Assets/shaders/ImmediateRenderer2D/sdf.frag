#version 300 es
precision mediump float; // Default precision for floats

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

in vec2 v_sdf_coord; // Received coordinate for SDF calculation (scaled local space, usually -0.5 to 0.5 range adjusted)

// Uniforms from C++
uniform int u_shape_type;    // 0 for Circle, 1 for Rectangle
uniform vec4 u_fill_color;   // Color for the shape's interior
uniform vec4 u_line_color;   // Color for the shape's outline
uniform float u_line_width;  // Width of the outline in world units
uniform highp vec2 u_world_size; // Original size of the shape in world units (used to scale line width correctly)

out vec4 frag_color; // Output fragment color

// Signed Distance Function for a Circle centered at origin with radius 0.5
// p: point coordinates (normalized, -0.5 to 0.5)
// Returns distance: < 0 inside, 0 on edge, > 0 outside
float circle_sdf(vec2 p)
{
    // distance from origin - radius
    return length(p) - 0.5;
}

// Signed Distance Function for a Rectangle centered at origin with half-size 'half_size'
// p: point coordinates (normalized, -0.5 to 0.5)
// half_size: half dimensions of the rectangle (e.g., vec2(0.5, 0.5) for a unit square)
// Returns distance: < 0 inside, 0 on edge, > 0 outside
float rectangle_sdf(vec2 p, vec2 half_size)
{
    // Vector from center to edge in each axis, shifted so inside is negative
    vec2 d = abs(p) - half_size;

    // Distance from the corner (if outside in both axes) + distance along the edge (if outside in only one axis)
    // length(max(d, 0.0)): Distance from the nearest point on the rectangle boundary (positive if outside, 0 if inside/on edge)
    // min(max(d.x, d.y), 0.0): Negative distance along the axis closest to the edge (negative if inside, 0 if outside/on edge)
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

void main()
{
    float dist; // Signed distance to the shape edge

    // Calculate the distance based on the shape type
    if (u_shape_type == 0) // Circle
    {
        dist = circle_sdf(v_sdf_coord);
    }
    else // Rectangle (assuming u_shape_type == 1)
    {
        // For a rectangle defined from -0.5 to 0.5, the half-size is (0.5, 0.5)
        dist = rectangle_sdf(v_sdf_coord, vec2(0.5));
    }

    // Convert line width from world units to SDF coordinate space.
    // We use the larger dimension of the world size to ensure the line width
    // is consistent regardless of aspect ratio, scaling it relative to the SDF space (-0.5 to 0.5).
    float line_width_in_sdf = u_line_width / max(u_world_size.x, u_world_size.y);

    // --- Anti-aliasing and Outline Logic ---

    // Inner edge threshold for the outline (inside the shape)
    float inner_edge = -line_width_in_sdf;
    // Smoothstep creates a smooth transition between fill and line colors within the outline width inside the shape.
    // fwidth(dist) provides an estimate of the distance change per pixel, used for anti-aliasing.
    float inner_mix = smoothstep(inner_edge, inner_edge + fwidth(dist), dist);
    // Mix between fill color (when inner_mix=0) and line color (when inner_mix=1)
    vec4 shape_color = mix(u_fill_color, u_line_color, inner_mix);

    // Outer edge threshold (the actual shape boundary)
    // Smoothstep creates a smooth transition from the shape color to transparent outside the shape boundary.
    float outer_mix = smoothstep(0.0, fwidth(dist), dist);
    // Mix between the calculated shape_color (fill or line) and fully transparent (vec4(0.0)).
    frag_color = mix(shape_color, vec4(0.0), outer_mix);

    // Discard fragments that are essentially transparent to avoid unnecessary processing/blending.
    if (frag_color.a < 0.01)
    {
        discard;
    }
}