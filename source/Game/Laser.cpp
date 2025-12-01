/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Laser.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 29, 2025
*/
#include "Laser.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Window.hpp"

Laser::Laser(Math::vec2 pos, double rot, Math::vec2 sc, Math::vec2 vel) : GameObject(pos, rot, sc)
{
    SetVelocity(vel);
    AddGOComponent(new CS230::Sprite("Assets/images/Laser.spt", this));
}

void Laser::Update(double dt)
{
    GameObject::Update(dt);

    const Math::vec2& pos       = GetPosition();
    Math::ivec2       isize     = GetGOComponent<CS230::Sprite>()->GetFrameSize();
    Math::vec2        size      = { static_cast<double>(isize.x), static_cast<double>(isize.y) };
    Math::vec2        half_size = size / 2.0;

    Math::ivec2 iwindow = Engine::GetWindow().GetSize();
    Math::vec2  window  = { static_cast<double>(iwindow.x), static_cast<double>(iwindow.y) };

    if (pos.y + half_size.y < 0 || pos.y - half_size.y > window.y || pos.x + half_size.x < 0 || pos.x - half_size.x > window.x)
    {
        Destroy();
    }
}

bool Laser::CanCollideWith(GameObjectTypes other_type)
{
    return other_type == GameObjectTypes::Meteor;
}

void Laser::ResolveCollision([[maybe_unused]] GameObject* other_object)
{
    if (other_object->Type() == GameObjectTypes::Meteor)
    {
        other_object->ResolveCollision(this);
    }

    Destroy();
}