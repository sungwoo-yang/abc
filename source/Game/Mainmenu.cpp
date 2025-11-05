/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mainmenu.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 29, 2025
*/

#include "Mainmenu.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Window.hpp"
#include "Fonts.hpp"
#include "Mode1.hpp"
#include "Mode2.hpp"
#include "States.hpp"

void MainMenu::update_text(int selected)
{
    for (size_t i = 0; i < menu_textures.size(); ++i)
    {
        unsigned int color = (i == static_cast<size_t>(selected)) ? 0xFFFFFFFF : 0x6B8E23FF;

        menu_textures[i].texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture(menu_textures[i].text, color);
    }
}

void MainMenu::Load()
{
    selected_index = 0;
    menu_textures.clear();

    double     spacing = 100.0;
    Math::vec2 center;
    center.x = static_cast<double>(Engine::GetWindow().GetSize().x) / 2.0;
    center.y = static_cast<double>(Engine::GetWindow().GetSize().y) / 3.0;

    menu_textures.push_back(
        {
            "Side Scroller", { center.x, center.y + spacing },
             nullptr
    });
    menu_textures.push_back(
        {
            "Space Shooter", { center.x, center.y },
             nullptr
    });
    menu_textures.push_back(
        {
            "Exit", { center.x, center.y - spacing },
             nullptr
    });

    title_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("CS230 Engine Test", 0x9370DBFF);

    update_text(selected_index);
}

void MainMenu::Update(double)
{
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Up))
    {
        --selected_index;
        if (selected_index < 0)
        {
            selected_index = static_cast<int>(menu_textures.size()) - 1;
        }
        update_text(selected_index);
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Down))
    {
        ++selected_index;
        if (selected_index >= static_cast<int>(menu_textures.size()))
        {
            selected_index = 0;
        }
        update_text(selected_index);
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Enter))
    {
        switch (selected_index)
        {
            case 0:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<Mode1>();
                break;
            case 1:
                Engine::GetGameStateManager().PopState();
                Engine::GetGameStateManager().PushState<Mode2>();
                break;
            case 2: Engine::GetGameStateManager().PopState(); break;
        }
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
    }
}

void MainMenu::Draw() const
{
    Engine::GetWindow().Clear(0x202020FF);

    for (const auto& item : menu_textures)
    {
        if (item.texture != nullptr)
        {
            Math::vec2 size     = Math::vec2(item.texture->GetSize());
            Math::vec2 draw_pos = Math::vec2(item.position) - size / 2.0f;
            item.texture->Draw(Math::TranslationMatrix(draw_pos));
        }
    }

    if (title_texture != nullptr)
    {
        Math::vec2 title_pos  = { static_cast<double>(Engine::GetWindow().GetSize().x) / 2.0, static_cast<double>(Engine::GetWindow().GetSize().y) - 170.0 };
        Math::vec2 title_size = Math::vec2(title_texture->GetSize());

        double scale  = 1.5;
        auto   matrix = Math::TranslationMatrix(title_pos - title_size * scale / 2.0) * Math::ScaleMatrix({ scale, scale });
        title_texture->Draw(matrix);
    }
}

void MainMenu::DrawImGui()
{
}

void MainMenu::Unload()
{
    for (auto& item : menu_textures)
    {
        item.texture = nullptr;
    }
    menu_textures.clear();

    title_texture = nullptr;
}
