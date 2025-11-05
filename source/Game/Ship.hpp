
/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 20, 2025
*/

#pragma once
#include "Engine/Sprite.hpp"
#include "Engine/Input.hpp"
#include "Engine/Vec2.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

class Ship : public CS230::GameObject {
public:
	Ship(Math::vec2 start_position);
	void Update(double dt) override;
	void Draw(Math::TransformationMatrix& camera_matrix) override;
	bool Exploded() const { return exploded; }
	GameObjectTypes Type() override { return GameObjectTypes::Ship; }
	std::string TypeName() override { return "Ship"; }
	bool CanCollideWith(GameObjectTypes other_type) override;
	void ResolveCollision(GameObject* other_object) override;

private:
	Math::ivec2 size;
	static constexpr double speed = 700;
	static constexpr double drag = 1;
	static constexpr double rotation_speed = 5;
	static constexpr double scale = 0.75;
	CS230::Sprite flame_left;
	CS230::Sprite flame_right;
	bool flame_on = false;
	bool exploded = false;

	enum class Animations {
		None,
		Explosion
	};
	
	enum class Flame_Animations {
		Off,
		On
	};
};