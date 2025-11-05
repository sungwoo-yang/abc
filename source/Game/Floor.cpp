/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Floor.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 27, 2025
*/

#include "Floor.hpp"

Floor::Floor(Math::irect boundary) : GameObject(static_cast<Math::vec2>(boundary.point_1)) {
    AddGOComponent(new CS230::RectCollision({ Math::ivec2{ 0, 0 }, boundary.Size() }, this));
}