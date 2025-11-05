/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Crates.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

class Crates : public CS230::GameObject {
public:
    Crates(Math::vec2 pos, int sz);
    GameObjectTypes Type() override { return GameObjectTypes::Crates; }
    std::string TypeName() override { return "Crates"; }
};
