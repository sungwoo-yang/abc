/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Background.hpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 3, 2025
*/

#pragma once
#include "Engine/Camera.hpp"
#include "Engine/Texture.hpp"

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
        std::shared_ptr<CS230::Texture> texture;
        double                          speed = 1;
    };

    std::vector<ParallaxLayer> backgrounds;
};
