/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.hpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 20, 2025
*/

#pragma once
#include "Engine/Camera.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/CountdownTimer.hpp"

class Cat : public CS230::GameObject
{
public:
    Cat(Math::vec2 start_position, GameObject* starting_floor);
    void Update(double dt) override;
    void Draw(const Math::TransformationMatrix& camera_matrix) override;

    const Math::vec2& GetPosition() const
    {
        return GameObject::GetPosition();
    };

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Cat;
    };

    std::string TypeName() override
    {
        return "Cat";
    }

    bool CanCollideWith(GameObjectTypes other_type) override;
    void ResolveCollision(GameObject* other_object) override;

private:
    static constexpr double x_acceleration    = 400;
    static constexpr double x_drag            = 450;
    static constexpr double max_velocity      = 300;
    static constexpr double jump_velocity     = 650;
    static constexpr double hurt_time         = 2.0;
    static constexpr double hurt_velocity     = 350;
    static constexpr double hurt_acceleration = 300;
    static constexpr double bounce_velocity   = 700;
    static constexpr double pop_velocity      = 400;
    static constexpr double LargeFallHeight   = 250;
    double                  fall_start_y;
    CS230::CountdownTimer*           hurt_timer = nullptr;
    GameObject*             standing_on;

    void update_x_velocity(double dt);

    enum class Animations
    {
        Idle,
        Running,
        Jumping,
        Falling,
        Skidding
    };

    class State_Jumping : public State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Jumping";
        }
    };

    State_Jumping state_jumping;

    class State_Idle : public State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Idle";
        }
    };

    State_Idle state_idle;

    class State_Falling : public State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Falling";
        }
    };

    State_Falling state_falling;

    class State_Running : public State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Running";
        }
    };

    State_Running state_running;

    class State_Skidding : public State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Skidding";
        }
    };

    State_Skidding state_skidding;
};
