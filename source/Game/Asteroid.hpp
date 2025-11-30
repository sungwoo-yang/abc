/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.hpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 24, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include "Engine/CountdownTimer.hpp"

class Asteroid : public CS230::GameObject
{
public:
    Asteroid(Math::vec2 start_position);

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Asteroid;
    }

    std::string TypeName() override
    {
        return "Asteroid";
    }

    void ResolveCollision(CS230::GameObject* other_object) override;
    bool CanCollideWith(GameObjectTypes type) override;

private:
    static constexpr double bounce_velocity = 500;

    enum class Animations
    {
        Land,
        None
    };

    class State_Bouncing : public State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Bouncing";
        }
    };

    State_Bouncing state_bouncing;

    class State_Landing : public State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Bouncing";
        }
    };

    State_Landing state_landing;
};
