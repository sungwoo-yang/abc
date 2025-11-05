/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Portal.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 27, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Collision.hpp"
#include "Engine/GameObjectTypes.hpp"

class Portal : public CS230::GameObject {
public:
    Portal(int ts, Math::irect boundary);
    std::string TypeName() override { return "Portal"; }
    virtual GameObjectTypes Type() override { return GameObjectTypes::Portal; }

    void GoToState();
private:
    int to_state;
};
