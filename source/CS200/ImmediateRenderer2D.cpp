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
    ImmediateRenderer2D::ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept
        : shader(std::move(other.shader)), vertex_array_object(other.vertex_array_object), vertex_buffer_object(other.vertex_buffer_object), element_buffer_object(other.element_buffer_object),
          viewProjectionMatrix(other.viewProjectionMatrix)
    {
        other.vertex_array_object   = 0;
        other.vertex_buffer_object  = 0;
        other.element_buffer_object = 0;
    }

    ImmediateRenderer2D& ImmediateRenderer2D::operator=(ImmediateRenderer2D&& other) noexcept
    {
        if (this != &other)
        {
            Shutdown();
            shader                = std::move(other.shader);
            vertex_array_object   = other.vertex_array_object;
            vertex_buffer_object  = other.vertex_buffer_object;
            element_buffer_object = other.element_buffer_object;
            viewProjectionMatrix  = other.viewProjectionMatrix;

            other.vertex_array_object   = 0;
            other.vertex_buffer_object  = 0;
            other.element_buffer_object = 0;
        }
        return *this;
    }

    ImmediateRenderer2D::~ImmediateRenderer2D()
    {
        Shutdown();
    }

    void ImmediateRenderer2D::Init()
    {
        shader = OpenGL::CreateShader(std::filesystem::path("Assets/shaders/ImmediateRenderer2D/quad.vert"), std::filesystem::path("Assets/shaders/ImmediateRenderer2D/quad.frag"));

        constexpr std::array<unsigned short, 6> indices = { 0, 1, 2, 2, 3, 0 };
        element_buffer_object                           = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indices }));

        constexpr std::array<float, 8> vertices = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
        vertex_buffer_object                    = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ vertices }));

        constexpr std::array<float, 8> texture_coordinates  = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
        OpenGL::BufferHandle           texture_coord_buffer = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ texture_coordinates }));


        vertex_array_object = OpenGL::CreateVertexArrayObject(
            {
                { vertex_buffer_object, { OpenGL::Attribute::Float2 } },
                { texture_coord_buffer, { OpenGL::Attribute::Float2 } }
        },
            element_buffer_object);
    }

    void ImmediateRenderer2D::Shutdown()
    {
        GL::DeleteVertexArrays(1, &vertex_array_object);
        GL::DeleteBuffers(1, &vertex_buffer_object);
        GL::DeleteBuffers(1, &element_buffer_object);
        OpenGL::DestroyShader(shader);

        vertex_array_object   = 0;
        vertex_buffer_object  = 0;
        element_buffer_object = 0;
    }

    void ImmediateRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        viewProjectionMatrix = view_projection;
    }

    void ImmediateRenderer2D::EndScene()
    {
    }

    void ImmediateRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        GL::UseProgram(shader.Shader);

        const auto& locations = shader.UniformLocations;
        GL::UniformMatrix3fv(locations.at("u_ndc_matrix"), 1, GL_FALSE, Renderer2DUtils::to_opengl_mat3(viewProjectionMatrix).data());
        GL::UniformMatrix3fv(locations.at("u_model_matrix"), 1, GL_FALSE, Renderer2DUtils::to_opengl_mat3(transform).data());

        Math::TransformationMatrix uv_matrix = Math::TranslationMatrix(texture_coord_bl) * Math::ScaleMatrix({ texture_coord_tr.x - texture_coord_bl.x, texture_coord_tr.y - texture_coord_bl.y });
        GL::UniformMatrix3fv(locations.at("u_uv_matrix"), 1, GL_FALSE, Renderer2DUtils::to_opengl_mat3(uv_matrix).data());

        auto color = Renderer2DUtils::unpack_color(tintColor);
        GL::Uniform4fv(locations.at("u_tint_color"), 1, color.data());

        GL::ActiveTexture(GL_TEXTURE0);
        GL::BindTexture(GL_TEXTURE_2D, texture);
        GL::Uniform1i(locations.at("u_texture"), 0);

        GL::BindVertexArray(vertex_array_object);
        GL::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

        GL::BindVertexArray(0);
        GL::UseProgram(0);
    }
}
