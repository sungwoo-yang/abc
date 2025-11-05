/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 20, 2025
*/

#include "Ship.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameObjectTypes.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/ShowCollision.hpp"
#include "Laser.hpp"
#include "ScreenWrap.hpp"

Ship::Ship(Math::vec2 start_position) : GameObject(start_position, 0.0, { scale, scale }), flame_left("Assets/Flame.spt"), flame_right("Assets/Flame.spt"), exploded(false)
{
    auto* sprite = new CS230::Sprite("Assets/Ship.spt", this);
    AddGOComponent(sprite);
    sprite->PlayAnimation(static_cast<int>(Animations::None));

    AddGOComponent(new ScreenWrap(*this));

    flame_left.PlayAnimation(static_cast<int>(Flame_Animations::Off));
    flame_right.PlayAnimation(static_cast<int>(Flame_Animations::Off));

    size     = sprite->GetFrameSize();
    flame_on = false;
}

void Ship::Update(double dt)
{
    if (exploded == false)
    {
        if (Engine::GetInput().KeyDown(CS230::Input::Keys::W))
        {
            UpdateVelocity(Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, speed * dt });
            if (flame_on == false)
            {
                flame_left.PlayAnimation(static_cast<int>(Flame_Animations::On));
                flame_right.PlayAnimation(static_cast<int>(Flame_Animations::On));
                flame_on = true;
            }
        }
        else
        {
            flame_left.PlayAnimation(static_cast<int>(Flame_Animations::Off));
            flame_right.PlayAnimation(static_cast<int>(Flame_Animations::Off));
            flame_on = false;
        }

        if (Engine::GetInput().KeyDown(CS230::Input::Keys::A))
        {
            UpdateRotation(rotation_speed * dt);
        }
        if (Engine::GetInput().KeyDown(CS230::Input::Keys::D))
        {
            UpdateRotation(-rotation_speed * dt);
        }

        if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Enter))
        {
            auto*      sprite         = GetGOComponent<CS230::Sprite>();
            Math::vec2 hotspot_left   = static_cast<Math::vec2>(sprite->GetHotSpot(3));
            Math::vec2 hotspot_right  = static_cast<Math::vec2>(sprite->GetHotSpot(4));
            Math::vec2 laser_velocity = Math::RotationMatrix(GetRotation()) * Math::vec2{ 0.0, Laser::DefaultVelocity };

            auto manager = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();
            manager->Add(new Laser(GetMatrix() * hotspot_left, GetRotation(), GetScale(), laser_velocity));
            manager->Add(new Laser(GetMatrix() * hotspot_right, GetRotation(), GetScale(), laser_velocity));
        }

        flame_left.Update(dt);
        flame_right.Update(dt);
    }

    UpdateVelocity(-GetVelocity() * drag * dt);
    UpdatePosition({ GetVelocity().x * dt, GetVelocity().y * dt });

    UpdateGOComponents(dt);
}

void Ship::Draw(const Math::TransformationMatrix& camera_matrix)
{
    auto* sprite = GetGOComponent<CS230::Sprite>();
    if (sprite == nullptr)
    {
        return;
    }

    if (!exploded)
    {
        flame_left.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(sprite->GetHotSpot(1)));
        flame_right.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(sprite->GetHotSpot(2)));
    }

    sprite->Draw(camera_matrix * GetMatrix());

    if (!exploded)
    {
        auto show = Engine::GetGameStateManager().GetGSComponent<CS230::ShowCollision>();
        if (show != nullptr && show->Enabled())
        {
            CS230::Collision* collision = GetGOComponent<CS230::Collision>();
            if (collision != nullptr)
            {
                collision->Draw(camera_matrix);
            }
        }
    }
}

bool Ship::CanCollideWith(GameObjectTypes)
{
    return true;
}

void Ship::ResolveCollision(GameObject* other_object)
{
    if (exploded)
        return;

    if (other_object->Type() == GameObjectTypes::Meteor)
    {
        auto* sprite = GetGOComponent<CS230::Sprite>();
        sprite->PlayAnimation(static_cast<int>(Animations::Explosion));
        RemoveGOComponent<CS230::Collision>();
        exploded = true;
    }
}