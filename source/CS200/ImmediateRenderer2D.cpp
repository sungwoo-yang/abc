/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
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
    struct QuadVertex
    {
        Math::vec2 pos;
        Math::vec2 uv;
    };

    // Private member variables
    struct ImmediateRenderer2D::Impl
    {
        OpenGL::VertexArrayHandle quadVAO = 0;
        OpenGL::BufferHandle quadVBO = 0;
        OpenGL::BufferHandle quadIBO = 0;
        OpenGL::CompiledShader quadShader{};
        Math::TransformationMatrix viewProjectionMatrix;
    };


    ImmediateRenderer2D::ImmediateRenderer2D() : pImpl(std::make_unique<Impl>()) {}

    ImmediateRenderer2D::ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept = default;

    ImmediateRenderer2D& ImmediateRenderer2D::operator=(ImmediateRenderer2D&& other) noexcept = default;

    ImmediateRenderer2D::~ImmediateRenderer2D()
    {
        Shutdown();
    }

    void ImmediateRenderer2D::Init()
    {
        // Quad vertices
        std::vector<QuadVertex> vertices = {
            {{-0.5f, -0.5f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f}, {0.0f, 1.0f}}
        };

        // Quad indices
        std::vector<unsigned char> indices = { 0, 1, 2, 2, 3, 0 };

        pImpl->quadVBO = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ vertices }));
        pImpl->quadIBO = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indices }));

        OpenGL::BufferLayout layout = {
            0,
            { OpenGL::Attribute::Float2, OpenGL::Attribute::Float2 }
        };

        pImpl->quadVAO = OpenGL::CreateVertexArrayObject({ {pImpl->quadVBO, layout} }, pImpl->quadIBO);

        pImpl->quadShader = OpenGL::CreateShader("Assets/shaders/ImmediateRenderer2D/quad.vert", "Assets/shaders/ImmediateRenderer2D/quad.frag");
    }

    void ImmediateRenderer2D::Shutdown()
    {
        if (pImpl->quadVAO != 0)
        {
            OpenGL::GL::DeleteVertexArrays(1, &pImpl->quadVAO);
            pImpl->quadVAO = 0;
        }
        if (pImpl->quadVBO != 0)
        {
            OpenGL::GL::DeleteBuffers(1, &pImpl->quadVBO);
            pImpl->quadVBO = 0;
        }
        if (pImpl->quadIBO != 0)
        {
            OpenGL::GL::DeleteBuffers(1, &pImpl->quadIBO);
            pImpl->quadIBO = 0;
        }
        OpenGL::DestroyShader(pImpl->quadShader);
    }

    void ImmediateRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        pImpl->viewProjectionMatrix = view_projection;
    }

    void ImmediateRenderer2D::EndScene()
    {
        // Nothing to do in immediate mode
    }

    void ImmediateRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        OpenGL::GL::UseProgram(pImpl->quadShader.Shader);

        auto model_mat = Renderer2DUtils::to_opengl_mat3(transform);
        auto ndc_mat = Renderer2DUtils::to_opengl_mat3(pImpl->viewProjectionMatrix);

        Math::TransformationMatrix uv_transform;
        uv_transform[0][0] = texture_coord_tr.x - texture_coord_bl.x;
        uv_transform[1][1] = texture_coord_tr.y - texture_coord_bl.y;
        uv_transform[0][2] = texture_coord_bl.x;
        uv_transform[1][2] = texture_coord_bl.y;
        auto uv_mat = Renderer2DUtils::to_opengl_mat3(uv_transform);


        OpenGL::GL::UniformMatrix3fv(pImpl->quadShader.UniformLocations.at("u_model_matrix"), 1, GL_FALSE, model_mat.data());
        OpenGL::GL::UniformMatrix3fv(pImpl->quadShader.UniformLocations.at("u_ndc_matrix"), 1, GL_FALSE, ndc_mat.data());
        OpenGL::GL::UniformMatrix3fv(pImpl->quadShader.UniformLocations.at("u_uv_matrix"), 1, GL_FALSE, uv_mat.data());


        auto tint = unpack_color(tintColor);
        OpenGL::GL::Uniform4fv(pImpl->quadShader.UniformLocations.at("u_tint_color"), 1, tint.data());

        OpenGL::GL::ActiveTexture(GL_TEXTURE0);
        OpenGL::GL::BindTexture(GL_TEXTURE_2D, texture);
        OpenGL::GL::Uniform1i(pImpl->quadShader.UniformLocations.at("u_texture"), 0);


        OpenGL::GL::BindVertexArray(pImpl->quadVAO);
        OpenGL::GL::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);


        OpenGL::GL::BindVertexArray(0);
        OpenGL::GL::UseProgram(0);
    }
        if (coord == TextureCoordinate::S || coord == TextureCoordinate::Both)
        {
            GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        }
        if (coord == TextureCoordinate::T || coord == TextureCoordinate::Both)
        {
            GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));
        }
        GL::BindTexture(GL_TEXTURE_2D, 0);
    }
}
