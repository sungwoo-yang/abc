/**
 * \file
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Collision.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine.hpp"
#include "GameObject.hpp"
#include "Logger.hpp"

CS230::RectCollision::RectCollision(Math::irect boundary, GameObject* object) : boundary(boundary), object(object)
{
}

Math::rect CS230::RectCollision::WorldBoundary()
{
    return { object->GetMatrix() * static_cast<Math::vec2>(boundary.point_1), object->GetMatrix() * static_cast<Math::vec2>(boundary.point_2) };
}

void CS230::RectCollision::Draw(const Math::TransformationMatrix& display_matrix)
{
    (void)display_matrix;

    auto& renderer = Engine::GetRenderer2D();
    Math::rect world_boundary = WorldBoundary();

    Math::vec2 bottom_left  = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Bottom() };
    Math::vec2 bottom_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Bottom() };
    Math::vec2 top_left     = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Top() };
    Math::vec2 top_right    = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Top() };

    renderer.DrawLine(top_left, top_right, CS200::WHITE, 1.0);
    renderer.DrawLine(top_right, bottom_right, CS200::WHITE, 1.0);
    renderer.DrawLine(bottom_left, bottom_left, CS200::WHITE, 1.0);
    renderer.DrawLine(bottom_right, top_left, CS200::WHITE, 1.0);
}

bool CS230::RectCollision::IsCollidingWith(GameObject* other_object)
{
    Collision* other_collider = other_object->GetGOComponent<Collision>();


    if (other_collider == nullptr)
    {
        Engine::GetLogger().LogError("No collision component found");
        return false;
    }


    if (other_collider->Shape() != CollisionShape::Rect)
    {
        Engine::GetLogger().LogError("Rect vs unsupported type");
        return false;
    }

    Math::rect rectangle_1 = WorldBoundary();
    Math::rect rectangle_2 = static_cast<RectCollision*>(other_collider)->WorldBoundary();

    if (rectangle_1.Right() > rectangle_2.Left() && rectangle_1.Left() < rectangle_2.Right() && rectangle_1.Top() > rectangle_2.Bottom() && rectangle_1.Bottom() < rectangle_2.Top())
    {
        return true;
    }
    return false;
}

bool CS230::RectCollision::IsCollidingWith(Math::vec2 point)
{
    Math::rect rect = WorldBoundary();

    return (point.x >= rect.Left() && point.x < rect.Right() && point.y >= rect.Bottom() && point.y <= rect.Top());
}

CS230::CircleCollision::CircleCollision(double radius, GameObject* object) : radius(radius), object(object)
{
}

void CS230::CircleCollision::Draw(const Math::TransformationMatrix& display_matrix)
{
    (void)display_matrix;

    auto& renderer = Engine::GetRenderer2D();

    Math::TransformationMatrix transform = Math::TranslationMatrix(object->GetPosition()) * Math::ScaleMatrix(Math::vec2{ GetRadius(), GetRadius() });

    renderer.DrawCircle(transform, CS200::CLEAR, CS200::WHITE, 1.0);
}

double CS230::CircleCollision::GetRadius()
{
    Math::vec2 scale = object->GetScale();
    return scale.x > scale.y ? radius * scale.x : radius * scale.y;
}

bool CS230::CircleCollision::IsCollidingWith(GameObject* other_object)
{
    Collision* other_collider = other_object->GetGOComponent<Collision>();

    if (other_collider == nullptr)
    {
        Engine::GetLogger().LogError("No collision component found");
        return false;
    }

    if (other_collider->Shape() != CollisionShape::Circle)
    {
        Engine::GetLogger().LogError("Rect vs unsupported type");
        return false;
    }

    CircleCollision* other      = dynamic_cast<CircleCollision*>(other_collider);
    Math::vec2       A          = object->GetPosition();
    Math::vec2       B          = other->object->GetPosition();
    double           dx         = A.x - B.x;
    double           dy         = A.y - B.y;
    double           distance   = dx * dx + dy * dy;
    double           sum_radius = GetRadius() + other->GetRadius();

    return distance <= sum_radius * sum_radius;
}

bool CS230::CircleCollision::IsCollidingWith(Math::vec2 point)
{
    Math::vec2 center = object->GetPosition();
    double     r      = GetRadius();

    Math::vec2 diff         = point - center;
    double     dist_squared = diff.x * diff.x + diff.y * diff.y;

    return dist_squared <= r * r;
}