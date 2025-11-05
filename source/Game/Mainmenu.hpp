/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mainmenu.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 29, 2025
*/

#pragma once
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Font.hpp"
#include "Engine/Vec2.hpp"
#include <vector>

class MainMenu : public CS230::GameState {
public:
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() const override;
    gsl::czstring GetName() const override {
        return "MainMenu"; 
    }

private:
    struct MenuTexture {
        std::string text;
        Math::vec2 position;
        CS230::Texture* texture = nullptr;
    };
        
    int selected_index = 0;
    std::vector<MenuTexture> menu_textures;
    CS230::Texture* title_texture = nullptr;

    void update_text(int selected);
};