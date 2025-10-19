#version 300 es
precision         mediump float;
precision mediump sampler2D;

/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
in vec2 v_TexCoord;

out vec4 o_FragColor;

uniform sampler2D u_Texture;
uniform vec4      u_TintColor;
uniform mat3      u_TextureTransform;

void main()
{
    vec2 transformed_uv = (u_TextureTransform * vec3(v_TexCoord, 1.0)).xy;
    o_FragColor = texture(u_Texture, transformed_uv) * u_TintColor;
}
