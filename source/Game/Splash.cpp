/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#include "Splash.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Window.hpp"
#include "Mainmenu.hpp"
#include "States.hpp"

Splash::Splash()
{
}

void Splash::Load()
{
    counter = 0;
    texture = Engine::GetTextureManager().Load("Assets/images/DigiPen.png");
}

void Splash::Update(double dt)
{
    counter += dt;
    Engine::GetLogger().LogDebug(std::to_string(counter));
    if (counter >= 3)
    {
        Engine::GetGameStateManager().PopState();
        Engine::GetGameStateManager().PushState<MainMenu>();
    }
}

void Splash::Unload()
{
}

void Splash::Draw() const
{
    Engine::GetWindow().Clear(UINT_MAX);

    texture->Draw(Math::TranslationMatrix({ (Engine::GetWindow().GetSize() - texture->GetSize()) / 2.0 }));
}
