/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Robot.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#pragma once
#include "../Engine/GameObject.h"
#include "../GameObjectTypes.h"
#include "Cat.h"

class Robot : public CS230::GameObject {
public:
    Robot(Math::vec2 position, Cat* cat, double left, double right);
    void Update(double dt) override;
    GameObjectTypes Type() override { return GameObjectTypes::Robot; }
    std::string TypeName() override { return "Robot"; }
    void ResolveCollision(GameObject* other_object) override;

    enum class Animations {
        Walking,
        Angry,
        Broken
    };

    static constexpr double walking_speed = 75.0;
    static constexpr double angry_speed = 105.0;
private:
    bool is_alive = true;
    Cat* target_cat;
    double left_boundary, right_boundary;
    double speed;

    class State_Walking : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Walking"; }
    };

    State_Walking state_walking;

    class State_Angry : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Angry"; }
    };

    State_Angry state_angry;

    class State_Broken : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Broken"; }
    };

    State_Broken state_broken;
};
