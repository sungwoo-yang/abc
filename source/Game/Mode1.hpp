/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.hpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#pragma once
#include "Engine/Font.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"

class Cat;

class Mode1 : public CS230::GameState
{
public:
    Mode1();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() const override;

    void DrawImGui() override
    {
    }

    gsl::czstring GetName() const override
    {
        return "Mode1";
    }

    static constexpr double floor     = 80;
    static constexpr double timer_max = 60;

private:
    Cat*                            cat_ptr;
    std::shared_ptr<CS230::Texture> timer_texture = nullptr;
    std::shared_ptr<CS230::Texture> score_texture = nullptr;
    int                             last_timer;
    void                            update_timer_text(int value);
    void                            update_score_text(int value);
};
