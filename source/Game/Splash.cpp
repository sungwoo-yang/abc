/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#include "Engine/Engine.hpp"
#include "States.hpp"
#include "Splash.hpp"



Splash::Splash() {

}

void Splash::Load() {
    counter = 0;
    texture = Engine::GetTextureManager().Load("Assets/DigiPen.png");
}

void Splash::Update(double dt) {
    counter += dt;
    Engine::GetLogger().LogDebug(std::to_string(counter));
    if (counter >= 3) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Splash::Unload() {

}

void Splash::Draw() {
    Engine::GetWindow().Clear(UINT_MAX);

    texture->Draw(Math::TranslationMatrix({ (Engine::GetWindow().GetSize() - texture->GetSize()) / 2.0 }));
}

