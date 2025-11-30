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
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Vec2.h"
#include "../Engine/GameObjectManager.h"
#include "../Engine/Timer.h"
#include "Ship.h"

class Mode2 : public CS230::GameState {
public:
    Mode2();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "Mode2";
    }

private:
    CS230::GameObjectManager* gameobjectmanager = nullptr;
    Ship* ship;
    CS230::Texture* gameover_text = nullptr;
    CS230::Texture* restart_text = nullptr;
    CS230::Texture* score_text = nullptr;
    CS230::Timer meteor_spawn_timer;
    void update_score_text(int value);
};
