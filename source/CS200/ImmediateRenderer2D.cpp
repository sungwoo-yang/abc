/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "ImmediateRenderer2D.hpp"

#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/GL.hpp"
#include "Renderer2DUtils.hpp"
#include <utility>

namespace CS200
{
    ImmediateRenderer2D::ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept : quad(std::exchange(other.quad, {})), quadShader(std::exchange(other.quadShader, {}))
    {
    }

    ImmediateRenderer2D& ImmediateRenderer2D::operator=(ImmediateRenderer2D&& other) noexcept
    {
        std::swap(quad, other.quad);
        std::swap(quadShader, other.quadShader);
        return *this;
    }

    ImmediateRenderer2D::~ImmediateRenderer2D()
    {
        Shutdown();
    }

    void ImmediateRenderer2D::Init()
    {
        struct Vertex
        {
            float x, y, u, v;
        };

        constexpr std::array vertices = {
            Vertex{ -0.5f, -0.5f, 0.0f, 0.0f },
            Vertex{ -0.5f,  0.5f, 0.0f, 1.0f },
            Vertex{  0.5f,  0.5f, 1.0f, 1.0f },
            Vertex{  0.5f, -0.5f, 1.0f, 0.0f }
        };
        constexpr std::array<unsigned char, 6> indices = { 0, 1, 2, 0, 2, 3 };

        quad.vertexBuffer = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ vertices }));
        quad.indexBuffer  = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indices }));

        const auto layout = {
            OpenGL::VertexBuffer{ quad.vertexBuffer, { OpenGL::Attribute::Float2, OpenGL::Attribute::Float2 } }
        };
        quad.vertexArray = OpenGL::CreateVertexArrayObject(layout, quad.indexBuffer);

        quadShader = OpenGL::CreateShader(std::filesystem::path{ "Assets/shaders/ImmediateRenderer2D/quad.vert" }, std::filesystem::path{ "Assets/shaders/ImmediateRenderer2D/quad.frag" });
    }

    void ImmediateRenderer2D::Shutdown()
    {
        GL::DeleteBuffers(1, &quad.vertexBuffer);
        GL::DeleteBuffers(1, &quad.indexBuffer);
        GL::DeleteVertexArrays(1, &quad.vertexArray);
        OpenGL::DestroyShader(quadShader);
        quad = {};
    }

    void ImmediateRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        GL::UseProgram(quadShader.Shader);
        const auto& locations     = quadShader.UniformLocations;
        const auto  to_ndc_opengl = Renderer2DUtils::to_opengl_mat3(view_projection);
        GL::UniformMatrix3fv(locations.at("u_ndc_matrix"), 1, GL_FALSE, to_ndc_opengl.data());
    }

    void ImmediateRenderer2D::EndScene()
    {
        GL::UseProgram(0);
    }

    void ImmediateRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        GL::BindVertexArray(quad.vertexArray);
        GL::BindTexture(GL_TEXTURE_2D, texture);

        const auto& locations    = quadShader.UniformLocations;
        const auto  model_matrix = Renderer2DUtils::to_opengl_mat3(transform);
        GL::UniformMatrix3fv(locations.at("u_model_matrix"), 1, GL_FALSE, model_matrix.data());

        Math::TransformationMatrix uv_transform;
        uv_transform[0][0]   = texture_coord_tr.x - texture_coord_bl.x;
        uv_transform[1][1]   = texture_coord_tr.y - texture_coord_bl.y;
        uv_transform[0][2]   = texture_coord_bl.x;
        uv_transform[1][2]   = texture_coord_bl.y;
        const auto uv_matrix = Renderer2DUtils::to_opengl_mat3(uv_transform);
        GL::UniformMatrix3fv(locations.at("u_uv_matrix"), 1, GL_FALSE, uv_matrix.data());

        const auto color = unpack_color(tintColor);
        GL::Uniform4fv(locations.at("u_tint_color"), 1, color.data());

        GL::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);

        GL::BindTexture(GL_TEXTURE_2D, 0);
        GL::BindVertexArray(0);
    }

    void ImmediateRenderer2D::DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Circle);
    }

    void ImmediateRenderer2D::DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(const Math::TransformationMatrix& transform, Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        const auto line_transform = Renderer2DUtils::CalculateLineTransform(transform, start_point, end_point, line_width);
        DrawSDF(line_transform, line_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width);
    }
}
