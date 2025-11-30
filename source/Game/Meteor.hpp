/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.hpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

class Meteor : public CS230::GameObject
{
public:
    Meteor();
    Meteor(Meteor* parent);

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Meteor;
    }

    std::string TypeName() override
    {
        return "Meteor";
    }

    void Update(double dt) override;
    void ResolveCollision(GameObject* other_object) override;

private:
    double                  health;
    int                     size;
    static constexpr double default_velocity  = 100;
    static constexpr int    default_size      = 2;
    static constexpr double default_healths[] = { 3, 5, 8 };
    static constexpr double default_scales[]  = { 0.5, 0.75, 1.0 };
    bool                    exploded          = false;

    enum class Animations
    {
        None,
        Fade
    };
};