/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Component.hpp"
#include "Rect.hpp"

namespace Math
{
    class TransformationMatrix;
}

namespace CS230
{
    class GameObject;

    class Collision : public Component
    {
    public:
        enum class CollisionShape
        {
            Rect,
            Circle
        };
        virtual CollisionShape Shape()                                                = 0;
        virtual void           Draw(const Math::TransformationMatrix& display_matrix) = 0;
        virtual bool           IsCollidingWith(GameObject* other_object)              = 0;
        virtual bool           IsCollidingWith(Math::vec2 point)                      = 0;
    };

    class RectCollision : public Collision
    {
    public:
        RectCollision(Math::irect bound, GameObject* obj);

        CollisionShape Shape() override
        {
            return CollisionShape::Rect;
        }

        void         Draw(const Math::TransformationMatrix& display_matrix) override;
        Math::rect   WorldBoundary();
        bool         IsCollidingWith(GameObject* other_object) override;
        virtual bool IsCollidingWith(Math::vec2 point) override;

    private:
        Math::irect boundary;
        GameObject* object;
    };

    class CircleCollision : public Collision
    {
    public:
        CircleCollision(double rad, GameObject* obj);

        CollisionShape Shape() override
        {
            return CollisionShape::Circle;
        }

        void         Draw(const Math::TransformationMatrix& display_matrix) override;
        double       GetRadius();
        bool         IsCollidingWith(GameObject* other_object) override;
        virtual bool IsCollidingWith(Math::vec2 point) override;

    private:
        double      radius;
        GameObject* object;
    };
}