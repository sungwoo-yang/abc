/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Greavity.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 8, 2025
*/

#pragma once
#include "../Engine/Component.h"

class Gravity : public CS230::Component {
public:
    Gravity(double value) : gravity(value) {}
    double GetValue() { return gravity; }
private:
    double gravity;
};
