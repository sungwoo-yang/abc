/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Floor.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 27, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Collision.hpp"
#include "Engine/GameObjectTypes.hpp"

class Floor : public CS230::GameObject {
public:
    Floor(Math::irect boundary);
    std::string TypeName() override { return "Floor"; }
    virtual GameObjectTypes Type() override { return GameObjectTypes::Floor; }
};