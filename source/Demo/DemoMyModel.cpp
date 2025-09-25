/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "DemoMyModel.hpp"
#include "CS200/NDC.hpp"
#include "CS200/Renderer2DUtils.hpp"
#include "CS200/RenderingAPI.hpp"
#include "DemoBufferTypes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Random.hpp"
#include "Engine/Window.hpp"
#include "OpenGL/Buffer.hpp"
#include "OpenGL/GL.hpp"
#include <cmath>
#include <imgui.h>
#include <numbers>
#include <stb_perlin.h>

void DemoMyModel::Load()
{
    loadShaders();
    createFishModel();
    fishes.resize(32);
    currentDisplaySize = Engine::GetWindowEnvironment().DisplaySize;
    regenerate_all_fishes();
}

void DemoMyModel::Unload()
{
    OpenGL::DestroyShader(backgroundShader);
    OpenGL::DestroyShader(fishShader);
    GL::DeleteBuffers(1, &uniformBlock), uniformBlock                                   = 0;
    GL::DeleteVertexArrays(1, &background.modelHandle), background.modelHandle          = 0;
    GL::DeleteVertexArrays(1, &fish.modelHandle), fish.modelHandle                      = 0;
    GL::DeleteBuffers(1, &background.vertexBufferHandle), background.vertexBufferHandle = 0;
    GL::DeleteBuffers(1, &fish.vertexBufferHandle), fish.vertexBufferHandle             = 0;
    GL::DeleteBuffers(1, &background.indexBufferHandle), background.indexBufferHandle   = 0;
    GL::DeleteBuffers(1, &fish.indexBufferHandle), fish.indexBufferHandle               = 0;
    background.indicesCount = 0;
    fish.indicesCount       = 0;
    backgroundVertices.clear();
    backgroundIndices.clear();
    fishes.clear();
}

void DemoMyModel::Update()
{
    // Check for display size changes
    const auto newDisplaySize = Engine::GetWindowEnvironment().DisplaySize;
    if (newDisplaySize.x != currentDisplaySize.x || newDisplaySize.y != currentDisplaySize.y)
    {
        currentDisplaySize = newDisplaySize;
        regenerate_all_fishes();
    }

    updateBackgroundModel();

    updateNDCUniformValues();

    updateFishes();
}

void DemoMyModel::Draw() const
{
    CS200::RenderingAPI::Clear();

    OpenGL::UpdateBufferData(OpenGL::BufferType::UniformBlocks, uniformBlock, std::as_bytes(std::span{ toNDC }));
    GL::BindBuffer(GL_UNIFORM_BUFFER, uniformBlock);

    GL::UseProgram(backgroundShader.Shader);
    drawObject(background);

    GL::UseProgram(fishShader.Shader);

    for (const auto& fishData : fishes)
    {
        if (!fishData.shouldDraw)
            continue;
        const auto                 fish_to_world    = Math::TranslationMatrix(fishData.position) * Math::RotationMatrix(fishData.rotation) * Math::ScaleMatrix(fishData.scale);
        const std::array<float, 9> model_opengl_mat = CS200::Renderer2DUtils::to_opengl_mat3(fish_to_world);

        GL::UniformMatrix3fv(fishShader.UniformLocations.at("uModel"), 1, GL_FALSE, model_opengl_mat.data());
        drawObject(fish);
    }
}

void DemoMyModel::DrawImGui()
{
    const auto timing = Engine::GetWindowEnvironment();

    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", static_cast<double>(timing.FPS));
        const std::size_t num_fishes = static_cast<std::size_t>(std::count_if(fishes.begin(), fishes.end(), [](const FishData& v) { return v.alive; }));
        ImGui::LabelText("Fishes", "%zu", num_fishes);

        if (num_fishes == MAX_NUMBER_FISH)
        {
            ImGui::BeginDisabled();
            ImGui::ArrowButton("+", ImGuiDir_Up);
            ImGui::EndDisabled();
        }
        else
        {
            if (const bool increase = ImGui::ArrowButton("+", ImGuiDir_Up); increase)
            {
                increase_number_of_fishes(num_fishes);
            }
        }
        ImGui::SameLine();
        if (num_fishes == 1)
        {
            ImGui::BeginDisabled();
            ImGui::ArrowButton("-", ImGuiDir_Down);
            ImGui::EndDisabled();
        }
        else
        {
            if (const bool decrease = ImGui::ArrowButton("-", ImGuiDir_Down); decrease)
            {
                decrease_number_of_fishes(num_fishes);
            }
        }

        if (ImGui::Button("Regenerate All"))
        {
            regenerate_all_fishes();
        }

        ImGui::Separator();
        if (ImGui::Button("Switch to STUDENTDEMO Demo"))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<DemoBufferTypes>();
        }
    }
    ImGui::End();
}

gsl::czstring DemoMyModel::GetName() const
{
    return "Demo My Model";
}

void DemoMyModel::loadShaders()
{
    using filepath   = std::filesystem::path;
    backgroundShader = OpenGL::CreateShader(filepath{ "Assets/shaders/DemoBufferTypes/background.vert" }, filepath{ "Assets/shaders/DemoBufferTypes/background.frag" });
    fishShader       = OpenGL::CreateShader(filepath{ "Assets/shaders/DemoBufferTypes/villager.vert" }, filepath{ "Assets/shaders/DemoBufferTypes/villager.frag" });
    uniformBlock     = OpenGL::CreateBuffer(OpenGL::BufferType::UniformBlocks, sizeof(toNDC));
    OpenGL::BindUniformBufferToShader(backgroundShader.Shader, 0, uniformBlock, "NDC");
    OpenGL::BindUniformBufferToShader(fishShader.Shader, 0, uniformBlock, "NDC");
}

void DemoMyModel::createFishModel()
{
    // A simple fish shape
    // x, y, r, g, b
    constexpr float vertices[] = {
        // Body
        -0.5f,
        0.0f,
        1.0f,
        0.5f,
        0.0f, // Pointy mouth
        0.0f,
        0.25f,
        1.0f,
        0.5f,
        0.0f, // Top
        0.0f,
        -0.25f,
        1.0f,
        0.5f,
        0.0f, // Bottom
        0.4f,
        0.0f,
        1.0f,
        0.5f,
        0.0f, // Back center
              // Tail
        0.4f,
        0.0f,
        1.0f,
        0.6f,
        0.0f,
        0.7f,
        0.2f,
        1.0f,
        0.6f,
        0.0f,
        0.7f,
        -0.2f,
        1.0f,
        0.6f,
        0.0f,
        // Eye
        -0.3f,
        0.05f,
        0.0f,
        0.0f,
        0.0f,
    };

    constexpr unsigned short indices[] = {
        0, 1, 3, // Top half of body
        0, 3, 2, // Bottom half of body
        4, 5, 6  // Tail
    };
    fish.vertexBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, std::as_bytes(std::span{ vertices }));
    fish.indexBufferHandle  = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, std::as_bytes(std::span{ indices }));
    fish.indicesCount       = static_cast<GLsizei>(std::ssize(indices));
    const auto layout       = OpenGL::VertexBuffer{ fish.vertexBufferHandle, OpenGL::BufferLayout{ { OpenGL::Attribute::Float2, OpenGL::Attribute::Float3 } } };
    fish.modelHandle        = OpenGL::CreateVertexArrayObject(layout, fish.indexBufferHandle);
}

void DemoMyModel::updateNDCUniformValues()
{
    const auto        display_size  = currentDisplaySize;
    const Math::ivec2 viewport_size = { static_cast<int>(display_size.x), static_cast<int>(display_size.y) };
    const auto        to_ndc        = CS200::build_ndc_matrix(viewport_size);
    const auto        as_3x3        = CS200::Renderer2DUtils::to_opengl_mat3(to_ndc);
    for (std::size_t col = 0; col < 3; ++col)
    {
        const std::size_t src_offset = col * 3;
        const std::size_t dst_offset = col * 4;

        toNDC[dst_offset + 0] = as_3x3[src_offset + 0];
        toNDC[dst_offset + 1] = as_3x3[src_offset + 1];
        toNDC[dst_offset + 2] = as_3x3[src_offset + 2];
        toNDC[dst_offset + 3] = 0.0f;
    }
}

void DemoMyModel::updateBackgroundModel()
{
    const auto window_size = Engine::GetWindow().GetSize();

    // Calculate grid dimensions
    const int  num_quads_wide        = window_size.x / QuadSize + 2;
    const int  num_quads_tall        = window_size.y / QuadSize + 2;
    const auto num_required_vertices = num_quads_wide * num_quads_tall * 4;

    // Create grid parameters (calculated once)
    const GridParams gridParams = { .numQuadsWide = num_quads_wide, .numQuadsTall = num_quads_tall };

    // Calculate animation parameters
    const auto animParams = calculateAnimationParameters();

    // Ensure buffers have sufficient capacity
    const bool indices_need_update = ensureBackgroundBuffersCapacity(gridParams);

    // Generate vertex data for all quads
    background.indicesCount    = num_quads_wide * num_quads_tall * 6;
    unsigned int vertex_index  = 0;
    unsigned int indices_index = 0;

    for (int row = 0; row < num_quads_tall; ++row)
    {
        for (int column = 0; column < num_quads_wide; ++column)
        {
            generateQuadVertices(row, column, gridParams, animParams);

            // Only update indices if we grew the buffer (they don't change during animation)
            if (indices_need_update)
            {
                backgroundIndices[indices_index + 0] = static_cast<unsigned short>(vertex_index + 0);
                backgroundIndices[indices_index + 1] = static_cast<unsigned short>(vertex_index + 1);
                backgroundIndices[indices_index + 2] = static_cast<unsigned short>(vertex_index + 2);
                backgroundIndices[indices_index + 3] = static_cast<unsigned short>(vertex_index + 0);
                backgroundIndices[indices_index + 4] = static_cast<unsigned short>(vertex_index + 2);
                backgroundIndices[indices_index + 5] = static_cast<unsigned short>(vertex_index + 3);
            }
            vertex_index += 4;
            indices_index += 6;
        }
    }

    // Update GPU buffers
    updateBackgroundBuffersOnGPU(num_required_vertices, indices_need_update);
}

void DemoMyModel::drawObject(const object& object)
{
    constexpr GLenum         primitive_pattern        = GL_TRIANGLES;
    constexpr GLenum         indices_type             = GL_UNSIGNED_SHORT;
    constexpr GLvoid*        byte_offset_into_indices = nullptr;
    constexpr OpenGL::Handle no_object                = 0;

    if (object.modelHandle == 0) [[unlikely]]
        return;

    GL::BindVertexArray(object.modelHandle);
    GL::DrawElements(primitive_pattern, object.indicesCount, indices_type, byte_offset_into_indices);

    GL::BindVertexArray(no_object);
}

DemoMyModel::AnimationParams DemoMyModel::calculateAnimationParameters()
{
    const float elapsed_time = static_cast<float>(Engine::GetWindowEnvironment().ElapsedTime);

    // Multiple sine waves with different frequencies for more complex animation
    const float wave1_t = (std::sin(elapsed_time * WaveFrequency1) + 1.0f) * 0.5f; // Primary wave
    const float wave2_t = (std::sin(elapsed_time * WaveFrequency2) + 1.0f) * 0.5f; // Faster secondary wave
    const float wave3_t = (std::sin(elapsed_time * WaveFrequency3) + 1.0f) * 0.5f; // Slower tertiary wave

    // Combine waves for more interesting motion
    const float combined_wave  = (wave1_t * WaveMix1 + wave2_t * WaveMix2 + wave3_t * WaveMix3);
    const float quad_tilt_size = -QuadSize / 4.0f * (1.0f - combined_wave) + combined_wave * QuadSize / 4.0f;

    return AnimationParams{
        .elapsedTime  = elapsed_time,
        .quadTiltSize = quad_tilt_size,
    };
}

bool DemoMyModel::ensureBackgroundBuffersCapacity(const GridParams& grid_params)
{
    const auto num_quads             = grid_params.numQuadsWide * grid_params.numQuadsTall;
    const auto num_required_vertices = num_quads * 4;
    const auto num_required_indices  = num_quads * 6;

    if (num_required_vertices > std::ssize(backgroundVertices))
    {
        // Clean up existing buffers
        GL::DeleteVertexArrays(1, &background.modelHandle);
        background.modelHandle = 0;
        GL::DeleteBuffers(1, &background.vertexBufferHandle);
        background.vertexBufferHandle = 0;
        GL::DeleteBuffers(1, &background.indexBufferHandle);
        background.indexBufferHandle = 0;
        background.indicesCount      = 0;

        // Resize CPU buffers
        backgroundVertices.resize(static_cast<std::size_t>(num_required_vertices));
        backgroundIndices.resize(static_cast<std::size_t>(num_required_indices));

        // Create new GPU buffers
        background.vertexBufferHandle = OpenGL::CreateBuffer(OpenGL::BufferType::Vertices, static_cast<GLsizeiptr>(backgroundVertices.size() * sizeof(backgroundVertices[0])));
        background.indexBufferHandle  = OpenGL::CreateBuffer(OpenGL::BufferType::Indices, static_cast<GLsizeiptr>(backgroundIndices.size() * sizeof(backgroundIndices[0])));

        const auto layout      = OpenGL::VertexBuffer{ background.vertexBufferHandle, OpenGL::BufferLayout{ { OpenGL::Attribute::Float2, OpenGL::Attribute::UByte4ToNormalized } } };
        background.modelHandle = OpenGL::CreateVertexArrayObject(layout, background.indexBufferHandle);

        return true; // New buffers need index data
    }
    return false; // No resize needed
}

void DemoMyModel::generateQuadVertices(int row, int column, const GridParams& grid_params, const AnimationParams& anim_params)
{
    const auto x = static_cast<float>(column * QuadSize - QuadSize);
    const auto y = static_cast<float>(row * QuadSize - QuadSize);

    const float tilt_amount = anim_params.quadTiltSize;

    // Generate noise-based color using class constants
    const float fx = x * NoiseFrequency;
    const float fy = y * NoiseFrequency;

    // HSV-based color animation for smoother, more vibrant transitions
    const float base_hue_noise = stb_perlin_fbm_noise3(fx, fy, anim_params.elapsedTime * 0.3f, NoiseLacunarity, NoiseGain, NoiseLayers);
    const float hue            = std::fmod(180.0f + base_hue_noise * 120.0f + anim_params.elapsedTime * 10.0f, 360.0f);

    // Keep saturation and value in pastel range for light, soft colors
    const float saturation_noise = stb_perlin_fbm_noise3(fx, fy, anim_params.elapsedTime * 0.2f + 10.0f, NoiseLacunarity, NoiseGain, NoiseLayers);
    const float saturation       = 0.3f + (saturation_noise * 0.1f + 0.1f) * 0.5f;

    const float value_noise = stb_perlin_fbm_noise3(fx, fy, anim_params.elapsedTime * 0.15f + 20.0f, NoiseLacunarity, NoiseGain, NoiseLayers);
    const float value       = 0.95f + (value_noise * 0.01f + 0.01f) * 0.03f;
    float       r_float = 0, g_float = 0, b_float = 0;
    ImGui::ColorConvertHSVtoRGB(hue / 360.0f, saturation, value, r_float, g_float, b_float);

    // Convert to 0-255 range for vertex colors
    const auto r = static_cast<unsigned char>(std::lround(r_float * 255.0f));
    const auto g = static_cast<unsigned char>(std::lround(g_float * 255.0f));
    const auto b = static_cast<unsigned char>(std::lround(b_float * 255.0f));

    const int vertex_index = (row * grid_params.numQuadsWide + column) * 4;

    // Create quad vertices
    const auto bottom_left = Vertex{
        .x = x, .y = y, .color = { r, g, b, 255 }
    };

    const auto bottom_right = Vertex{
        .x = x + QuadSize, .y = y, .color = { r, g, b, 255 }
    };

    const auto top_right = Vertex{
        .x = x + QuadSize + tilt_amount, .y = y + QuadSize, .color = { r, g, b, 255 }
    };

    const auto top_left = Vertex{
        .x = x + tilt_amount, .y = y + QuadSize, .color = { r, g, b, 255 }
    };

    backgroundVertices[static_cast<std::size_t>(vertex_index + 0)] = bottom_left;
    backgroundVertices[static_cast<std::size_t>(vertex_index + 1)] = bottom_right;
    backgroundVertices[static_cast<std::size_t>(vertex_index + 2)] = top_right;
    backgroundVertices[static_cast<std::size_t>(vertex_index + 3)] = top_left;
}

void DemoMyModel::updateBackgroundBuffersOnGPU(int num_required_vertices, bool indices_need_update) const
{
    // Update vertex buffer
    GL::BindBuffer(GL_ARRAY_BUFFER, background.vertexBufferHandle);
    GL::BufferData(GL_ARRAY_BUFFER, num_required_vertices * static_cast<GLsizeiptr>(sizeof(backgroundVertices[0])), nullptr, GL_STREAM_DRAW);
    GL::BindBuffer(GL_ARRAY_BUFFER, 0);

    const auto vertex_data_span = std::span{ backgroundVertices.data(), static_cast<std::size_t>(num_required_vertices) };
    OpenGL::UpdateBufferData(OpenGL::BufferType::Vertices, background.vertexBufferHandle, std::as_bytes(vertex_data_span));

    // Only update index buffer if it grew (indices don't change during animation)
    if (indices_need_update)
    {
        const auto index_data_span = std::span{ backgroundIndices.data(), static_cast<std::size_t>(background.indicesCount) };
        OpenGL::UpdateBufferData(OpenGL::BufferType::Indices, background.indexBufferHandle, std::as_bytes(index_data_span));
    }
}

void DemoMyModel::increase_number_of_fishes(std::size_t current_size)
{
    const auto  next_size = current_size << 1;
    const auto  size      = currentDisplaySize;
    std::size_t count     = 0;
    for (auto& fishData : fishes)
    {
        if (fishData.alive)
        {
            ++count;
        }
        else
        {
            fishData.alive      = true;
            fishData.shouldDraw = true;
            generateRandomFish(fishData, size);
            if (++count >= next_size)
            {
                return;
            }
        }
    }
    while (count < next_size)
    {
        fishes.emplace_back();
        generateRandomFish(fishes.back(), size);
        ++count;
    }
}

void DemoMyModel::decrease_number_of_fishes(std::size_t current_size)
{
    const auto next_size        = static_cast<long>(current_size >> 1);
    const auto offscreen_length = currentDisplaySize.Length() * 0.75;
    for (auto iter_fish = fishes.begin() + next_size; iter_fish != fishes.end(); ++iter_fish)
    {
        auto& fishData          = *iter_fish;
        fishData.alive          = false;
        const auto to_offscreen = (fishData.position - 0.5 * currentDisplaySize).Normalize();
        fishData.targetPosition += to_offscreen * offscreen_length;
    }
}

void DemoMyModel::regenerate_all_fishes()
{
    const auto size = currentDisplaySize;
    for (auto& fishData : fishes)
    {
        if (!fishData.alive)
            continue;
        generateRandomFish(fishData, size);
    }
}

void DemoMyModel::generateRandomFish(FishData& fishData, const Math::vec2& screenSize) const
{
    const double scale   = util::random(32.0, 256.0);
    fishData.targetScale = Math::vec2{ scale, scale };

    const double margin       = scale * 0.5;
    fishData.targetPosition.x = util::random(margin, screenSize.x - margin);
    fishData.targetPosition.y = util::random(margin, screenSize.y - margin);

    // Random rotation between 0 and 2 PI radians
    fishData.targetRotation = util::random(0.0, 2.0 * std::numbers::pi);

    fishData.positionOffset.x = fishData.targetPosition.x;
    fishData.positionOffset.y = fishData.targetPosition.y;
    fishData.scaleOffset      = Math::vec2{ scale, scale };
    fishData.rotationSpeed    = util::random(-1.0, 1.0);
    fishData.positionSpeed.x  = util::random(0.3, 0.8);
    fishData.positionSpeed.y  = util::random(0.2, 0.6);
    fishData.scaleSpeed.x     = util::random(0.5, 1.2);
    fishData.scaleSpeed.y     = util::random(0.4, 1.0);
    fishData.timeOffset       = util::random(0.0, std::numbers::pi * 2.0);
}

template <typename T>
void ease_to_target(T& current, const T& target, double delta_time, double weight = 1.0)
{
    const auto easing = std::min(delta_time * weight, 1.0);
    current += easing * (target - current);
}

void DemoMyModel::updateFishes()
{
    const auto   environment  = Engine::GetWindowEnvironment();
    const double elapsed_time = environment.ElapsedTime;

    for (auto& fishData : fishes)
    {
        if (fishData.alive)
        {
            const double unique_time = elapsed_time + fishData.timeOffset;

            // Animate position with gentle circular/figure-8 motion
            const double pos_x_wave = std::sin(unique_time * fishData.positionSpeed.x) * 50.0;
            const double pos_y_wave = std::cos(unique_time * fishData.positionSpeed.y) * 30.0;

            fishData.targetPosition.x = fishData.positionOffset.x + pos_x_wave;
            fishData.targetPosition.y = fishData.positionOffset.y + pos_y_wave;

            // Animate scale with gentle breathing effect
            const double scale_wave_x = std::sin(unique_time * fishData.scaleSpeed.x) * 20.0;
            const double scale_wave_y = std::cos(unique_time * fishData.scaleSpeed.y) * 15.0;

            fishData.targetScale.x = fishData.scaleOffset.x + scale_wave_x;
            fishData.targetScale.y = fishData.scaleOffset.y + scale_wave_y;

            // Ensure minimum scale
            fishData.targetScale.x = std::max(fishData.targetScale.x, 32.0);
            fishData.targetScale.y = std::max(fishData.targetScale.y, 32.0);

            // Animate rotation
            fishData.targetRotation += fishData.rotationSpeed * environment.DeltaTime;
        }
        else if (fishData.shouldDraw)
        {
            const Math::vec2     fish_min   = fishData.position - 0.5 * fishData.scale;
            const Math::vec2     fish_max   = fishData.position + 0.5 * fishData.scale;
            constexpr Math::vec2 screen_min = { 0.0, 0.0 };
            const Math::vec2     screen_max = currentDisplaySize;
            if (fish_min.x >= screen_max.x || fish_max.x <= screen_min.x || fish_min.y >= screen_max.y || fish_max.y <= screen_min.y)
            {
                fishData.shouldDraw = false;
                fishData.position   = { 0.0, 0.0 };
                continue;
            }
        }

        if (fishData.shouldDraw == true)
        {
            ease_to_target(fishData.position, fishData.targetPosition, environment.DeltaTime, 1.2);
            ease_to_target(fishData.scale, fishData.targetScale, environment.DeltaTime, 2.0);
            ease_to_target(fishData.rotation, fishData.targetRotation, environment.DeltaTime);
        }
    }
}