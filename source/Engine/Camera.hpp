/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Component.hpp"
#include "Matrix.hpp"
#include "Rect.hpp"
#include "Vec2.hpp"

namespace CS230
{
    class Camera : public Component
    {
    public:
        Camera(Math::rect player_zone);
        void                       SetPosition(Math::vec2 new_position);
        const Math::vec2&          GetPosition() const;
        void                       SetLimit(Math::irect new_limit);
        void                       Update(const Math::vec2& player_position);
        Math::TransformationMatrix GetMatrix();

    private:
        Math::irect limit;
        Math::vec2  position;
        Math::rect  player_zone;
    };
}