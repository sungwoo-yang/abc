/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#include "Mode2.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Particle.hpp"
#include "Engine/ShowCollision.hpp"
#include "Engine/Texture.hpp"
#include "Engine/Window.hpp"
#include "Fonts.hpp"
#include "Meteor.hpp"
#include "Mode2.hpp"
#include "Particles.hpp"
#include "Score.hpp"
#include "States.hpp"
#include "ship.hpp"

Mode2::Mode2() : gameover_text(nullptr), restart_text(nullptr), ship(nullptr), meteor_spawn_timer(4.5)
{
}

void Mode2::Load()
{
    gameobjectmanager = new CS230::GameObjectManager();
    AddGSComponent(gameobjectmanager);

    ship = new Ship({ static_cast<double>(Engine::GetWindow().GetSize().x / 2), static_cast<double>(Engine::GetWindow().GetSize().y / 2) });

    gameobjectmanager->Add(ship);
#ifdef _DEBUG
    AddGSComponent(new CS230::ShowCollision());
#endif
    AddGSComponent(new CS230::ParticleManager<Particles::Hit>());
    AddGSComponent(new CS230::ParticleManager<Particles::MeteorBit>());
    AddGSComponent(new Score());
    update_score_text(0);
}

void Mode2::Update(double dt)
{
    UpdateGSComponents(dt);

    if (ship && ship->Exploded())
    {
        if (gameover_text == nullptr || restart_text == nullptr)
        {
            gameover_text = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Game Over!", 0xFFFFFFFF);
            restart_text  = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Press R To Restart", 0xFFFFFFFF);
        }

        if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R))
        {
            Engine::GetGameStateManager().PopState();
            Engine::GetGameStateManager().PushState<Mode2>();
        }
    }

    meteor_spawn_timer.Update(dt);
    if (meteor_spawn_timer.Remaining() <= 0)
    {
        meteor_spawn_timer.Reset();
        gameobjectmanager->Add(new Meteor());
    }

    gameobjectmanager->UpdateAll(dt);

    update_score_text(Engine::GetGameStateManager().GetGSComponent<Score>()->Value());

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
    }
}

void Mode2::Draw() const
{
    Engine::GetWindow().Clear(0x000000FF);

    auto& renderer = Engine::GetRenderer2D();
    renderer.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    gameobjectmanager->DrawAll(Math::TransformationMatrix());

    if (ship && ship->Exploded() && gameover_text && restart_text)
    {
        Math::ivec2 win = Engine::GetWindow().GetSize();
        gameover_text->Draw(Math::TranslationMatrix(Math::ivec2{ win.x / 2 - gameover_text->GetSize().x / 2, win.y / 2 + 80 }));
        restart_text->Draw(Math::TranslationMatrix(Math::ivec2{ win.x / 2 - restart_text->GetSize().x / 2, win.y / 5 }));
    }

    if (score_text)
    {
        score_text->Draw(Math::TranslationMatrix(Math::ivec2{ 10, Engine::GetWindow().GetSize().y - score_text->GetSize().y - 10 }));
    }

    renderer.EndScene();
}

void Mode2::Unload()
{
    if (gameobjectmanager)
    {
        gameobjectmanager->Unload();
    }
    ClearGSComponents();
}

void Mode2::update_score_text(int value)
{
    score_text = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Score: " + std::to_string(value), 0xFFFFFFFF);
}