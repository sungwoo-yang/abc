/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mainmenu.hpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 29, 2025
*/

#pragma once
#include "Engine/Font.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include <memory>
#include <vector>

class MainMenu : public CS230::GameState
{
public:
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() const override;

    void DrawImGui() override
    {
    }

    gsl::czstring GetName() const override
    {
        return "MainMenu";
    }

private:
    struct MenuTexture
    {
        std::string                     text;
        Math::vec2                      position;
        std::shared_ptr<CS230::Texture> texture;
    };

    int                             selected_index = 0;
    std::vector<MenuTexture>        menu_textures;
    std::shared_ptr<CS230::Texture> title_texture;

    void update_text(int selected);
};