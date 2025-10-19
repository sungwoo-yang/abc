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
    namespace
    {
        struct QuadVertex
        {
            float x, y;
            float u, v;
        };

        constexpr std::array<QuadVertex, 4> QuadVertices = {
            QuadVertex{ -0.5f, -0.5f, 0.0f, 0.0f },
            QuadVertex{ +0.5f, -0.5f, 1.0f, 0.0f },
            QuadVertex{ +0.5f, +0.5f, 1.0f, 1.0f },
            QuadVertex{ -0.5f, +0.5f, 0.0f, 1.0f }
        };

        constexpr std::array<unsigned short, 6> QuadIndices = { 0, 1, 2, 2, 3, 0 };

        struct SDFVertex
        {
            float x, y;
        };

        constexpr std::array<SDFVertex, 4> SDFVertices = {
            SDFVertex{ -0.5f, -0.5f },
            SDFVertex{ +0.5f, -0.5f },
            SDFVertex{ +0.5f, +0.5f },
            SDFVertex{ -0.5f, +0.5f }
        };

        struct CameraUniforms
        {
            Renderer2DUtils::mat3 u_ViewProjectionMatrix;
        };
    }

    ImmediateRenderer2D::ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept
        : cameraUBO(std::exchange(other.cameraUBO, 0)), quadShader(std::move(other.quadShader)), quadVAO(std::exchange(other.quadVAO, 0)), quadVBO(std::exchange(other.quadVBO, 0)),
          quadIBO(std::exchange(other.quadIBO, 0)), sdfShader(std::move(other.sdfShader)), sdfVAO(std::exchange(other.sdfVAO, 0)), sdfVBO(std::exchange(other.sdfVBO, 0))
    {
    }

    ImmediateRenderer2D& ImmediateRenderer2D::operator=(ImmediateRenderer2D&& other) noexcept
    {
        if (this == &other)
            return *this;

        Shutdown();

        cameraUBO  = std::exchange(other.cameraUBO, 0);
        quadShader = std::move(other.quadShader);
        quadVAO    = std::exchange(other.quadVAO, 0);
        quadVBO    = std::exchange(other.quadVBO, 0);
        quadIBO    = std::exchange(other.quadIBO, 0);
        sdfShader  = std::move(other.sdfShader);
        sdfVAO     = std::exchange(other.sdfVAO, 0);
        sdfVBO     = std::exchange(other.sdfVBO, 0);

        return *this;
    }

    ImmediateRenderer2D::~ImmediateRenderer2D()
    {
        Shutdown();
    }

    void ImmediateRenderer2D::Init()
    {
        quadShader = OpenGL::CreateShader(std::filesystem::path("Assets/shaders/ImmediateRenderer2D/quad.vert"), std::filesystem::path("Assets/shaders/ImmediateRenderer2D/quad.frag"));
        quadVBO    = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span(QuadVertices)));
        quadIBO    = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span(QuadIndices)));

        quadVAO = OpenGL::CreateVertexArrayObject(
            OpenGL::VertexBuffer{
                quadVBO, OpenGL::BufferLayout{ OpenGL::Attribute::Float2, OpenGL::Attribute::Float2 }
        },
            quadIBO);

        sdfShader = OpenGL::CreateShader(std::filesystem::path("Assets/shaders/ImmediateRenderer2D/sdf.vert"), std::filesystem::path("Assets/shaders/ImmediateRenderer2D/sdf.frag"));
        sdfVBO    = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span(SDFVertices)));

        sdfVAO    = OpenGL::CreateVertexArrayObject(OpenGL::VertexBuffer{ sdfVBO, OpenGL::BufferLayout{ OpenGL::Attribute::Float2 } }, quadIBO);
        cameraUBO = OpenGL::CreateBuffer(OpenGL::BufferType::UniformBlocks, sizeof(CameraUniforms));

        const GLuint cameraBindingPoint = 0;
        OpenGL::BindUniformBufferToShader(quadShader.Shader, cameraBindingPoint, cameraUBO, "Camera");
        OpenGL::BindUniformBufferToShader(sdfShader.Shader, cameraBindingPoint, cameraUBO, "Camera");
    }

    void ImmediateRenderer2D::Shutdown()
    {
        OpenGL::DestroyShader(quadShader);
        OpenGL::DestroyShader(sdfShader);

        GL::DeleteVertexArrays(1, &quadVAO);
        GL::DeleteVertexArrays(1, &sdfVAO);

        std::array<BufferHandle, 4> buffers = { quadVBO, quadIBO, sdfVBO, cameraUBO };
        GL::DeleteBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());

        quadVAO = sdfVAO = quadVBO = quadIBO = sdfVBO = cameraUBO = 0;
    }

    void ImmediateRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        CameraUniforms cameraData;
        cameraData.u_ViewProjectionMatrix = Renderer2DUtils::to_opengl_mat3(view_projection);

        OpenGL::UpdateBufferData(OpenGL::BufferType::UniformBlocks, cameraUBO, std::as_bytes(std::span(&cameraData, 1)));
    }

    void ImmediateRenderer2D::EndScene()
    {
    }

    void ImmediateRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        GL::UseProgram(quadShader.Shader);

        GL::UniformMatrix3fv(quadShader.UniformLocations.at("u_ModelMatrix"), 1, GL_FALSE, Renderer2DUtils::to_opengl_mat3(transform).data());

        auto color = CS200::unpack_color(tintColor);
        GL::Uniform4fv(quadShader.UniformLocations.at("u_TintColor"), 1, color.data());

        Math::ScaleMatrix          uv_scale(Math::vec2{ texture_coord_tr.x - texture_coord_bl.x, texture_coord_tr.y - texture_coord_bl.y });
        Math::TranslationMatrix    uv_translate(Math::vec2{ texture_coord_bl.x, texture_coord_bl.y });
        Math::TransformationMatrix uv_transform = uv_translate * uv_scale;
        GL::UniformMatrix3fv(quadShader.UniformLocations.at("u_TextureTransform"), 1, GL_FALSE, Renderer2DUtils::to_opengl_mat3(uv_transform).data());

        GL::Uniform1i(quadShader.UniformLocations.at("u_Texture"), 0);

        GL::ActiveTexture(GL_TEXTURE0);
        GL::BindTexture(GL_TEXTURE_2D, texture);

        GL::BindVertexArray(quadVAO);

        GL::DrawElements(GL_TRIANGLES, static_cast<GLsizei>(QuadIndices.size()), GL_UNSIGNED_SHORT, nullptr);
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

    void ImmediateRenderer2D::DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape)
    {
        GL::UseProgram(sdfShader.Shader);

        auto sdf_transform = Renderer2DUtils::CalculateSDFTransform(transform, line_width);
        GL::UniformMatrix3fv(sdfShader.UniformLocations.at("u_ModelMatrix"), 1, GL_FALSE, sdf_transform.QuadTransform.data());

        auto fill = CS200::unpack_color(fill_color);
        GL::Uniform4fv(sdfShader.UniformLocations.at("u_FillColor"), 1, fill.data());
        auto line = CS200::unpack_color(line_color);
        GL::Uniform4fv(sdfShader.UniformLocations.at("u_LineColor"), 1, line.data());
        GL::Uniform1f(sdfShader.UniformLocations.at("u_LineWidth"), static_cast<float>(line_width));
        GL::Uniform1i(sdfShader.UniformLocations.at("u_ShapeType"), static_cast<int>(sdf_shape));
        GL::Uniform2fv(sdfShader.UniformLocations.at("u_WorldSize"), 1, sdf_transform.WorldSize.data());
        GL::Uniform2fv(sdfShader.UniformLocations.at("u_QuadSize"), 1, sdf_transform.QuadSize.data());

        GL::BindVertexArray(sdfVAO);
        GL::DrawElements(GL_TRIANGLES, static_cast<GLsizei>(QuadIndices.size()), GL_UNSIGNED_SHORT, nullptr);
    }
}
