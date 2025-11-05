/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#pragma once
#include "Engine/CountdownTimer.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Vec2.hpp"
#include "Ship.hpp"

class Mode2 : public CS230::GameState
{
public:
    Mode2();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() const override;
    void DrawImGui() override;

    gsl::czstring GetName() const override
    {
        return "Mode2";
    }

private:
    CS230::GameObjectManager*       gameobjectmanager = nullptr;
    std::shared_ptr<CS230::Texture> gameover_text     = nullptr;
    std::shared_ptr<CS230::Texture> restart_text      = nullptr;
    Ship*                           ship;
    std::shared_ptr<CS230::Texture> score_text = nullptr;
    CS230::CountdownTimer           meteor_spawn_timer;
    void                            update_score_text(int value);
};
