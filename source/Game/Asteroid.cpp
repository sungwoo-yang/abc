/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 24, 2025
*/

#include "Asteroid.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Collision.hpp"
#include "Gravity.hpp"
#include "Mode1.hpp"

Asteroid::Asteroid(Math::vec2 start_position) : GameObject(start_position)
{
    AddGOComponent(new CS230::Sprite("Assets/Asteroid.spt", this));
    change_state(&state_landing);
}

void Asteroid::State_Bouncing::Enter(GameObject* object)
{
    Asteroid* asteroid = static_cast<Asteroid*>(object);
    asteroid->SetVelocity({ 0, bounce_velocity });
    asteroid->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void Asteroid::State_Bouncing::Update(GameObject* object, double dt)
{
    Asteroid* asteroid = static_cast<Asteroid*>(object);
    asteroid->UpdateVelocity({ 0, -Engine::GetGameStateManager().GetGSComponent<Gravity>()->GetValue() * dt });
}

void Asteroid::State_Bouncing::CheckExit([[maybe_unused]] GameObject* object)
{
}

void Asteroid::State_Landing::Enter(GameObject* object)
{
    Asteroid* asteroid = static_cast<Asteroid*>(object);
    asteroid->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Land));
}

void Asteroid::State_Landing::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
}

void Asteroid::State_Landing::CheckExit(GameObject* object)
{
    Asteroid* asteroid = static_cast<Asteroid*>(object);
    if (asteroid->GetGOComponent<CS230::Sprite>()->AnimationEnded())
    {
        asteroid->change_state(&asteroid->state_bouncing);
    }
}

bool Asteroid::CanCollideWith(GameObjectTypes type)
{
    return type == GameObjectTypes::Floor;
}

void Asteroid::ResolveCollision(CS230::GameObject* other_object)
{
    if (other_object->Type() == GameObjectTypes::Floor)
    {
        CS230::RectCollision* collider = other_object->GetGOComponent<CS230::RectCollision>();
        SetPosition({ GetPosition().x, collider->WorldBoundary().Top() + 2 });
        SetVelocity({ GetVelocity().x, 0 });
        change_state(&state_landing);
    }
}