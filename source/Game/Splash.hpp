/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#pragma once
#include "Engine/Engine.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"

class Splash : public CS230::GameState
{
public:
    Splash();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() const override;
    void DrawImGui() override;

    gsl::czstring GetName() const override
    {
        return "Splash";
    }

public:
    double          counter = 0;
    std::shared_ptr<CS230::Texture> texture = nullptr;
};
