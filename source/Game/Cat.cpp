/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 20, 2025
*/

#include "Cat.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Particle.hpp"
#include "Engine/Window.hpp"
#include "Gravity.hpp"
#include "Mode1.hpp"
#include "Particles.hpp"
#include "Portal.hpp"
#include "Score.hpp"
#include "States.hpp"

Cat::Cat(Math::vec2 start_position, GameObject* starting_floor) : GameObject(start_position), standing_on(starting_floor), hurt_timer(nullptr), fall_start_y(0)
{
    AddGOComponent(new CS230::Sprite("Assets/images/Cat.spt", this));
    change_state(&state_idle);
    hurt_timer = new CS230::CountdownTimer(0.0);
    AddGOComponent(hurt_timer);
}

void Cat::Update(double dt)
{
    GameObject::Update(dt);
    // Boundary Check
    auto       collider = GetGOComponent<CS230::RectCollision>();
    Math::rect boundary = collider->WorldBoundary();

    double cam_x = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x;
    double win_w = Engine::GetWindow().GetSize().x;

    double boundary_width = boundary.Right() - boundary.Left();

    if (boundary.Left() < cam_x)
    {
        SetPosition({ cam_x + boundary_width / 2.0, GetPosition().y });
        SetVelocity({ 0.0, GetVelocity().y });
    }

    if (boundary.Right() > cam_x + win_w)
    {
        SetPosition({ cam_x + win_w - boundary_width / 2.0, GetPosition().y });
        SetVelocity({ 0.0, GetVelocity().y });
    }
}

void Cat::Draw(const Math::TransformationMatrix& camera_matrix)
{
    if (hurt_timer == nullptr || hurt_timer->Remaining() == 0.0 || hurt_timer->TickTock() == true)
    {
        CS230::GameObject::Draw(camera_matrix);
    }
}

void Cat::update_x_velocity(double dt)
{
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        UpdateVelocity({ x_acceleration * dt, 0 });
        if (GetVelocity().x > max_velocity)
        {
            SetVelocity({ max_velocity, GetVelocity().y });
        }
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        UpdateVelocity({ -x_acceleration * dt, 0 });
        if (GetVelocity().x < -max_velocity)
        {
            SetVelocity({ -max_velocity, GetVelocity().y });
        }
    }
    else
    {
        if (GetVelocity().x > x_drag * dt)
        {
            UpdateVelocity({ -x_drag * dt, 0 });
        }
        else if (GetVelocity().x < -x_drag * dt)
        {
            UpdateVelocity({ x_drag * dt, 0 });
        }
        else
        {
            SetVelocity({ 0, GetVelocity().y });
        }
    }
}

void Cat::ResolveCollision(GameObject* other_object)
{
    Math::rect cat_rect   = GetGOComponent<CS230::RectCollision>()->WorldBoundary();
    Math::rect other_rect = other_object->GetGOComponent<CS230::RectCollision>()->WorldBoundary();

    switch (other_object->Type())
    {
        case GameObjectTypes::Floor: [[fallthrough]];
        case GameObjectTypes::Crates:
            {
                if (current_state == &state_falling && cat_rect.Top() > other_rect.Top() && other_object->IsCollidingWith(GetPosition()))
                {
                    double fall_distance = fall_start_y - GetPosition().y;
                    if (fall_distance >= LargeFallHeight)
                    {
                        Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, GetPosition(), { 0, 0 }, { 0, 100 }, PI / 2);
                    }

                    SetPosition({ GetPosition().x, other_rect.Top() });
                    standing_on = other_object;
                    current_state->CheckExit(this);
                    return;
                }

                if (cat_rect.Left() < other_rect.Right() && cat_rect.Right() > other_rect.Right())
                {
                    double overlap = other_rect.Right() - cat_rect.Left();
                    SetPosition({ GetPosition().x + overlap, GetPosition().y });
                    SetVelocity({ 0.0, GetVelocity().y });
                }
                else if (cat_rect.Right() > other_rect.Left() && cat_rect.Left() < other_rect.Left())
                {
                    double overlap = cat_rect.Right() - other_rect.Left();
                    SetPosition({ GetPosition().x - overlap, GetPosition().y });
                    SetVelocity({ 0.0, GetVelocity().y });
                }
            }
            break;
        case GameObjectTypes::Robot:
            if (current_state == &state_falling && cat_rect.Top() > other_rect.Top() && hurt_timer->Remaining() == 0)
            {
                SetPosition({ GetPosition().x, other_rect.Top() });
                SetVelocity({ GetVelocity().x, pop_velocity });
                Engine::GetGameStateManager().GetGSComponent<Score>()->Add(1);
                Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, GetPosition(), { 0, 0 }, { 0, 100 }, 2 * PI / 3);
                other_object->ResolveCollision(this);
            }
            else
            {
                if (cat_rect.Left() < other_rect.Left())
                {
                    SetVelocity({ -hurt_acceleration, hurt_velocity });
                }
                else
                {
                    SetVelocity({ hurt_acceleration, hurt_velocity });
                }
                change_state(&state_falling);
                hurt_timer->Set(hurt_time);
            }
            break;
        case GameObjectTypes::Asteroid:
            if (current_state == &state_falling && cat_rect.Top() > other_rect.Top() && hurt_timer->Remaining() == 0)
            {
                SetPosition({ GetPosition().x, other_rect.Top() });
                SetVelocity({ GetVelocity().x, bounce_velocity });
                other_object->ResolveCollision(this);
            }
            else
            {
                if (cat_rect.Left() < other_rect.Left())
                {
                    SetVelocity({ -hurt_acceleration, hurt_velocity });
                }
                else
                {
                    SetVelocity({ hurt_acceleration, hurt_velocity });
                }

                change_state(&state_falling);
                hurt_timer->Set(hurt_time);
            }
            break;
        case GameObjectTypes::Portal: static_cast<Portal*>(other_object)->GoToState(); break;
    }
}

bool Cat::CanCollideWith(GameObjectTypes other_type)
{
    if (other_type == GameObjectTypes::Particle)
    {
        return false;
    }

    return true;
}

// Jumping
void Cat::State_Jumping::Enter(GameObject* object)
{
    Cat* cat         = static_cast<Cat*>(object);
    cat->standing_on = nullptr;
    cat->SetVelocity({ cat->GetVelocity().x, Cat::jump_velocity });
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Jumping));
}

void Cat::State_Jumping::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->UpdateVelocity({ 0, -Engine::GetGameStateManager().GetGSComponent<Gravity>()->GetValue() * dt });
    cat->update_x_velocity(dt);
}

void Cat::State_Jumping::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Up) == false)
    {
        cat->change_state(&cat->state_falling);
        cat->SetVelocity({ cat->GetVelocity().x, 0 });
    }
    else if (cat->GetVelocity().y <= 0)
    {
        cat->change_state(&cat->state_falling);
    }
}

// Idle
void Cat::State_Idle::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Idle));
}

void Cat::State_Idle::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
}

void Cat::State_Idle::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
    {
        cat->change_state(&cat->state_jumping);
    }
}

// Falling
void Cat::State_Falling::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Falling));
    cat->fall_start_y = cat->GetPosition().y;
}

void Cat::State_Falling::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->UpdateVelocity({ 0, -Engine::GetGameStateManager().GetGSComponent<Gravity>()->GetValue() * dt });
    cat->update_x_velocity(dt);
}

void Cat::State_Falling::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat->standing_on != nullptr)
    {
        if (cat->GetVelocity().x > 0)
        {
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
            {
                cat->change_state(&cat->state_skidding);
            }
            else
            {
                cat->change_state(&cat->state_running);
            }
        }
        else if (cat->GetVelocity().x < 0)
        {
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
            {
                cat->change_state(&cat->state_skidding);
            }
            else
            {
                cat->change_state(&cat->state_running);
            }
        }
        else
        {
            cat->change_state(&cat->state_idle);
        }

        cat->SetVelocity({ cat->GetVelocity().x, 0 });
    }

    // if (cat->GetPosition().y < -500)
    // {
    //     Engine::GetGameStateManager().PopState();
    // }
}

// Running
void Cat::State_Running::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        cat->SetScale({ -1, 1 });
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        cat->SetScale({ 1, 1 });
    }
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Running));
}

void Cat::State_Running::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->update_x_velocity(dt);
}

void Cat::State_Running::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetVelocity().x == 0)
    {
        cat->change_state(&cat->state_idle);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
    {
        cat->change_state(&cat->state_jumping);
    }
    else if (cat->GetVelocity().x > 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left) || cat->GetVelocity().x < 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        cat->change_state(&cat->state_skidding);
    }

    if (cat->standing_on != nullptr && cat->standing_on->IsCollidingWith(cat->GetPosition()) == false)
    {
        cat->standing_on = nullptr;
        cat->change_state(&cat->state_falling);
    }
}

// Skidding
void Cat::State_Skidding::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Skidding));
    Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, cat->GetPosition(), { 0, 0 }, { 2 * cat->GetVelocity().x, 0 }, PI / 6);
}

void Cat::State_Skidding::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetVelocity().x > 0)
    {
        cat->UpdateVelocity({ -(x_drag + x_acceleration) * dt, 0 });
        if (cat->GetVelocity().x < 0)
        {
            cat->SetVelocity({ 0, cat->GetVelocity().y });
        }
    }
    else if (cat->GetVelocity().x < 0)
    {
        cat->UpdateVelocity({ (x_drag + x_acceleration) * dt, 0 });
        if (cat->GetVelocity().x > 0)
        {
            cat->SetVelocity({ 0, cat->GetVelocity().y });
        }
    }
}

void Cat::State_Skidding::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetVelocity().x == 0)
    {
        cat->change_state(&cat->state_idle);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
    {
        cat->change_state(&cat->state_jumping);
    }
    else if (cat->GetVelocity().x < 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        cat->change_state(&cat->state_running);
    }
    else if (cat->GetVelocity().x > 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        cat->change_state(&cat->state_running);
    }
}