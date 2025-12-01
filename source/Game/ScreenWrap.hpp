/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ScreenWrap.hpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 8, 2025
*/
#pragma once
#include "Engine/Component.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Sprite.hpp"

class ScreenWrap : public CS230::Component {
public:
    ScreenWrap(CS230::GameObject& obj) : object(obj) {}
    void Update(double dt) override;
private:
    CS230::GameObject& object;
};
