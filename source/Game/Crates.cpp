/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Crates.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#include "Crates.hpp"

Crates::Crates(Math::vec2 position, int size) : CS230::GameObject(position) {
    switch (size) {
    case 1:
        AddGOComponent(new CS230::Sprite("Assets/Crates1.spt", this));
        break;
    case 2:
        AddGOComponent(new CS230::Sprite("Assets/Crates2.spt", this));
        break;
    case 3:
        AddGOComponent(new CS230::Sprite("Assets/Crates3.spt", this));
        break;
    case 5:
        AddGOComponent(new CS230::Sprite("Assets/Crates5.spt", this));
        break;
    }
}
