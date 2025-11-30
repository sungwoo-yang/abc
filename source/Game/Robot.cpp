/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Robot.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#include "Robot.h"
#include "../Engine/Collision.h"
#include "../Engine/Engine.h"
#include "Mode1.h"

Robot::Robot(Math::vec2 position, Cat* cat, double left, double right) : 
	CS230::GameObject(position), target_cat(cat), left_boundary(left), right_boundary(right), speed(walking_speed) {
	AddGOComponent(new CS230::Sprite("Assets/Robot.spt", this));
    change_state(&state_walking);
}

void Robot::Update(double dt) {
    if (current_state) {
        current_state->Update(this, dt);
        current_state->CheckExit(this);
    }
    GameObject::Update(dt);
}

void Robot::ResolveCollision(GameObject* other_object) {
	if (other_object->Type() == GameObjectTypes::Cat) {
        SetVelocity({ 0, 0 });
		RemoveGOComponent<CS230::Collision>();
        change_state(&state_broken);
	}
}

// Walking
void Robot::State_Walking::Enter(GameObject* object) {
    Robot* robot = static_cast<Robot*>(object);
    robot->speed = walking_speed;
    robot->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Walking));
}
void Robot::State_Walking::Update(GameObject* object, [[maybe_unused]]double dt) {
    Robot* robot = static_cast<Robot*>(object);
    double dir = robot->GetScale().x;
    robot->SetVelocity({ robot->speed * dir, 0 });

    double x = robot->GetPosition().x;
    if ((x < robot->left_boundary && dir < 0) || (x > robot->right_boundary && dir > 0)) {
        robot->SetScale({ -dir, 1 });
    }
}
void Robot::State_Walking::CheckExit(GameObject* object) {
    Robot* robot = static_cast<Robot*>(object);
    double cat_x = robot->target_cat->GetPosition().x;
    double robot_x = robot->GetPosition().x;
    bool cat_in_range = (cat_x >= robot->left_boundary && cat_x <= robot->right_boundary);
    bool facing_cat = ((cat_x < robot_x && robot->GetScale().x < 0) ||
        (cat_x > robot_x && robot->GetScale().x > 0));
    if (cat_in_range && facing_cat) {
        robot->change_state(&robot->state_angry);
    }
}

// Angry
void Robot::State_Angry::Enter(GameObject* object) {
    Robot* robot = static_cast<Robot*>(object);
    robot->speed = angry_speed;
    robot->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Angry));
}
void Robot::State_Angry::Update(GameObject* object, [[maybe_unused]] double dt) {
    Robot* robot = static_cast<Robot*>(object);
    double dir = robot->GetScale().x;
    robot->SetVelocity({ robot->speed * dir, 0 });

    double x = robot->GetPosition().x;
    if ((x < robot->left_boundary && dir < 0) || (x > robot->right_boundary && dir > 0)) {
        robot->SetScale({ -dir, 1 });
    }
}
void Robot::State_Angry::CheckExit(GameObject* object) {
    Robot* robot = static_cast<Robot*>(object);
    double cat_x = robot->target_cat->GetPosition().x;
    double robot_x = robot->GetPosition().x;
    bool cat_in_range = (cat_x >= robot->left_boundary && cat_x <= robot->right_boundary);
    bool facing_cat = ((cat_x < robot_x && robot->GetScale().x < 0) ||
        (cat_x > robot_x && robot->GetScale().x > 0));
    if (!(cat_in_range && facing_cat)) {
        robot->change_state(&robot->state_walking);
    }
}

// Broken
void Robot::State_Broken::Enter(GameObject* object) {
    Robot* robot = static_cast<Robot*>(object);
    robot->SetVelocity(Math::vec2{ 0, 0 });
    robot->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Broken));
}
void Robot::State_Broken::Update([[maybe_unused]] GameObject* object, [[maybe_unused]]double dt) {}
void Robot::State_Broken::CheckExit([[maybe_unused]] GameObject* object) {}