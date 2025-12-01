/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#include "Mode1.hpp"
#include "Asteroid.hpp"
#include "Background.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Cat.hpp"
#include "Crates.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Particle.hpp"
#include "Engine/ShowCollision.hpp"
#include "Engine/Window.hpp"
#include "Floor.hpp"
#include "Fonts.hpp"
#include "Gravity.hpp"
#include "Particles.hpp"
#include "Portal.hpp"
#include "Robot.hpp"
#include "Score.hpp"
#include "States.hpp"

Mode1::Mode1() : cat_ptr(nullptr), last_timer(static_cast<int>(timer_max))
{
}

void Mode1::update_timer_text(int value)
{
    timer_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Timer: " + std::to_string(value), 0xFFFFFFFF);
}

void Mode1::update_score_text(int value)
{
    score_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Score: " + std::to_string(value), 0xFFFFFFFF);
}

void Mode1::Load()
{
    AddGSComponent(new CS230::Camera(
        {
            { 0.15 * Engine::GetWindow().GetSize().x, 0 },
            { 0.35 * Engine::GetWindow().GetSize().x, 0 }
    }));
    AddGSComponent(new Gravity(800.0));
    AddGSComponent(new CS230::CountdownTimer(timer_max));
    AddGSComponent(new Background());
    AddGSComponent(new CS230::GameObjectManager());
#ifdef _DEBUG
    AddGSComponent(new CS230::ShowCollision());
#endif

    auto background = Engine::GetGameStateManager().GetGSComponent<Background>();
    background->Add("Assets/images/Planets.png", 0.25);
    background->Add("Assets/images/Ships.png", 0.5);
    background->Add("Assets/images/Foreground.png", 1.0);

    auto camera = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>();
    camera->SetPosition({ 0, 0 });
    camera->SetLimit(
        {
            { 0, 0 },
            background->GetSize() - Engine::GetWindow().GetSize()
    });
    update_timer_text(last_timer);

    AddGSComponent(new Score());
    update_score_text(0);

    auto   object             = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();
    Floor* starting_floor_ptr = new Floor(
        Math::irect{
            {   0,                       0 },
            { 930, static_cast<int>(floor) }
    });
    object->Add(starting_floor_ptr);
    object->Add(new Floor(
        Math::irect{
            { 1014,                       0 },
            { 2700, static_cast<int>(floor) }
    }));
    object->Add(new Floor(
        Math::irect{
            { 2884,                       0 },
            { 4126, static_cast<int>(floor) }
    }));
    object->Add(new Floor(
        Math::irect{
            { 4208,                       0 },
            { 5760, static_cast<int>(floor) }
    }));

    object->Add(new Portal(
        static_cast<int>(States::MainMenu), Math::irect{
                                                { 5700,       static_cast<int>(floor) },
                                                { 5800, static_cast<int>(floor + 200) }
    }));

    cat_ptr = new Cat({ 300, floor }, starting_floor_ptr);
    object->Add(cat_ptr);
    object->Add(new Asteroid({ 600, floor }));
    object->Add(new Asteroid({ 1800, floor }));
    object->Add(new Asteroid({ 2400, floor }));
    object->Add(new Crates({ 900, floor }, 2));
    object->Add(new Crates({ 1400, floor }, 1));
    object->Add(new Crates({ 1900, floor }, 5));
    object->Add(new Crates({ 4000, floor }, 3));
    object->Add(new Crates({ 5400, floor }, 1));
    object->Add(new Crates({ 5500, floor }, 3));
    object->Add(new Crates({ 5600, floor }, 5));
    object->Add(new Robot({ 1025, floor }, cat_ptr, 1025, 1350));
    object->Add(new Robot({ 2050, floor }, cat_ptr, 2050, 2325));
    object->Add(new Robot({ 3400, floor }, cat_ptr, 3400, 3800));
    object->Add(new Robot({ 4225, floor }, cat_ptr, 4225, 4800));

    AddGSComponent(new CS230::ParticleManager<Particles::Smoke>());
}

void Mode1::Update(double dt)
{
    UpdateGSComponents(dt);
    Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->Update(cat_ptr->GetPosition());
    Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);

    if (Engine::GetGameStateManager().GetGSComponent<CS230::CountdownTimer>()->RemainingInt() < last_timer)
    {
        last_timer = Engine::GetGameStateManager().GetGSComponent<CS230::CountdownTimer>()->RemainingInt();
        update_timer_text(last_timer);
    }
    if (last_timer == 0)
    {
        Engine::GetGameStateManager().PopState();
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().PopState();
    }

    update_score_text(Engine::GetGameStateManager().GetGSComponent<Score>()->Value());
}

void Mode1::Draw() const
{
    Engine::GetWindow().Clear(0x000000FF);

    auto& renderer = Engine::GetRenderer2D();
    renderer.BeginScene(CS200::build_ndc_matrix(Engine::GetWindow().GetSize()));

    Engine::GetGameStateManager().GetGSComponent<Background>()->Draw(*Engine::GetGameStateManager().GetGSComponent<CS230::Camera>());
    Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->DrawAll(Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetMatrix());

    if (timer_texture)
    {
        timer_texture->Draw(
            Math::TranslationMatrix(Math::ivec2{ Engine::GetWindow().GetSize().x - 10 - timer_texture->GetSize().x, Engine::GetWindow().GetSize().y - timer_texture->GetSize().y - 5 }));
    }
    if (score_texture)
    {
        score_texture->Draw(
            Math::TranslationMatrix(
                Math::ivec2{ Engine::GetWindow().GetSize().x - 10 - score_texture->GetSize().x,
                             Engine::GetWindow().GetSize().y - (timer_texture ? timer_texture->GetSize().y : 0) - score_texture->GetSize().y - 10 }));
    }

    renderer.EndScene();
}

void Mode1::Unload()
{
    Engine::GetGameStateManager().GetGSComponent<Background>()->Unload();
    Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Unload();
    cat_ptr = nullptr;
    ClearGSComponents();
}
