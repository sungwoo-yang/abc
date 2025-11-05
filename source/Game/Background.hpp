#pragma once
#include "Engine/Camera.hpp"
#include "Engine/Texture.hpp"
#include <memory>
#include <vector>

class Background : public CS230::Component
{
public:
    void        Add(const std::filesystem::path& texture_path, double speed);
    void        Unload();
    void        Draw(const CS230::Camera& camera);
    Math::ivec2 GetSize();

private:
    struct ParallaxLayer
    {
        std::shared_ptr<CS230::Texture> texture = nullptr;
        double                          speed   = 1;
    };

    std::vector<ParallaxLayer> backgrounds;
};