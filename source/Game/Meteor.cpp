/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#include "Meteor.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Particle.hpp"
#include "Engine/Window.hpp"
#include "Particles.hpp"
#include "Score.hpp"

Meteor::Meteor() : Meteor(nullptr)
{
}

Meteor::Meteor(Meteor* parent) : CS230::GameObject({ 0, 0 })
{
    AddGOComponent(new CS230::Sprite("Assets/images/Meteor.spt", this));

    if (parent == nullptr)
    {
        Math::ivec2 win    = Engine::GetWindow().GetSize();
        Math::vec2  center = { win.x / 2.0, win.y / 2.0 };
        Math::vec2  spawn_pos;

        int    side = rand() % 4;
        double x    = static_cast<double>(rand() % win.x);
        double y    = static_cast<double>(rand() % win.y);

        switch (side)
        {
            case 0: spawn_pos = Math::vec2{ x, -50.0 }; break;
            case 1: spawn_pos = Math::vec2{ x, static_cast<double>(win.y) + 50.0 }; break;
            case 2: spawn_pos = Math::vec2{ -50.0, y }; break;
            case 3: spawn_pos = Math::vec2{ static_cast<double>(win.x) + 50.0, y }; break;
        }

        SetPosition(spawn_pos);
        SetVelocity((center - spawn_pos).Normalize() * default_velocity);
        SetRotation(((double)rand() / RAND_MAX) * 2 * PI);

        size = default_size;
    }
    else
    {
        SetVelocity(parent->GetVelocity());
        SetPosition(parent->GetPosition());
        SetRotation(parent->GetRotation());

        size = parent->size - 1;
    }
    health = default_healths[size];
    SetScale(Math::vec2{ default_scales[size], default_scales[size] });
}

void Meteor::Update(double dt)
{
    GameObject::Update(dt);

    auto* sprite = GetGOComponent<CS230::Sprite>();
    if (exploded && sprite->CurrentAnimation() == static_cast<int>(Animations::Fade) && sprite->AnimationEnded())
    {
        Destroy();
    }

    Math::vec2  pos    = GetPosition();
    Math::ivec2 screen = Engine::GetWindow().GetSize();

    double half_width  = GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2;
    double half_height = GetGOComponent<CS230::Sprite>()->GetFrameSize().y / 2;

    if (pos.x + half_width < 0)
    {
        pos.x = screen.x + half_width;
    }

    else if (pos.x - half_width > screen.x)
    {
        pos.x = -half_width;
    }

    if (pos.y + half_height < 0)
    {
        pos.y = screen.y + half_height;
    }

    else if (pos.y - half_height > screen.y)
    {
        pos.y = -half_height;
    }

    SetPosition(pos);
}

void Meteor::ResolveCollision(GameObject* other_object)
{
    UpdateVelocity(0.01 * other_object->GetVelocity());
    health -= 1;

    if (other_object->Type() == GameObjectTypes::Laser)
    {
        Math::vec2 meteor_center = GetPosition();
        Math::vec2 laser_pos     = other_object->GetPosition();

        auto       sprite        = GetGOComponent<CS230::Sprite>();
        Math::vec2 frame_size    = static_cast<Math::vec2>(sprite->GetFrameSize());
        double     approx_radius = frame_size.x / 2.0 * GetScale().x;

        Math::vec2 normal  = (laser_pos - meteor_center).Normalize();
        Math::vec2 hit_pos = meteor_center + normal * approx_radius;

        Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Hit>>()->Emit(1, hit_pos, { 0, 0 }, normal * 100, PI / 6);

        Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::MeteorBit>>()->Emit(5, hit_pos, { 0, 0 }, normal * 300, PI / 3);

        other_object->Destroy();
    }

    if (health <= 0 && !exploded)
    {
        exploded = true;

        RemoveGOComponent<CS230::Collision>();
        GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Fade));
        if (GetGOComponent<CS230::Sprite>()->AnimationEnded())
            Destroy();

        if (size > 0)
        {
            Meteor* meteor_1 = new Meteor(this);
            meteor_1->SetVelocity(Math::RotationMatrix(PI / 6) * GetVelocity());

            Meteor* meteor_2 = new Meteor(this);
            meteor_2->SetVelocity(Math::RotationMatrix(-PI / 6) * GetVelocity());

            Engine::GetGameStateManager().GetGSComponent<Score>()->Add(10);

            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(meteor_1);
            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(meteor_2);
        }
    }
}