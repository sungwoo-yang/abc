/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Portal.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 27, 2025
*/

#include "Portal.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "States.hpp"
#include "Mainmenu.hpp"
#include "Mode1.hpp"

Portal::Portal(int ts, Math::irect boundary) :
    GameObject(static_cast<Math::vec2>(boundary.point_1)),
    to_state(ts)
{
    AddGOComponent(new CS230::RectCollision({ Math::ivec2{ 0, 0 }, boundary.Size() }, this));
}

void Portal::GoToState() {
    Engine::GetGameStateManager().PopState();

    switch (static_cast<States>(to_state))
    {
    case States::MainMenu:
        Engine::GetGameStateManager().PushState<MainMenu>();
        break;
    case States::Mode1:
        Engine::GetGameStateManager().PushState<Mode1>();
        break;
    default:
        Engine::GetGameStateManager().PushState<MainMenu>();
        break;
    }
}
