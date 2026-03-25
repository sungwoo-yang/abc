#include "Camera.h"

Camera::Camera(void)
{
    eye = Point(0.0f, 0.0f, 0.0f);

    right = Vector(1.0f, 0.0f, 0.0f);
    up = Vector(0.0f, 1.0f, 0.0f);
    back = Vector(0.0f, 0.0f, 1.0f);

    distance = 1.0f;
    width = distance * 2.0f;
    height = width;

    near = 0.1f;
    far = 2.0f;
}

Camera::Camera(const Point &E, const Vector &look, const Vector &rel,
               float fov, float aspect, float near, float far)
    : eye(E), distance(1.0f), near(near), far(far)
{
    back = normalize(-look);
    right = normalize(cross(look, rel));
    up = cross(back, right);

    width = std::tan(fov / 2.0f) * distance * 2.0f;
    height = width / aspect;
}

Point Camera::Eye(void) const
{
    return eye;
}

Vector Camera::Right(void) const
{
    return right;
}

Vector Camera::Up(void) const
{
    return up;
}

Vector Camera::Back(void) const
{
    return back;
}

Vector Camera::ViewportGeometry(void) const
{
    return Vector(width, height, distance);
}

float Camera::NearDistance(void) const
{
    return near;
}

float Camera::FarDistance(void) const
{
    return far;
}

Camera &Camera::Zoom(float factor)
{
    width *= factor;
    height *= factor;

    return *this;
}

Camera &Camera::Forward(float distance_increment)
{
    Vector d = distance_increment * back;
    eye = eye - d;

    return *this;
}

Camera &Camera::Yaw(float angle)
{
    Affine R = rotate(angle, up);
    right = R * right;
    back = R * back;

    return *this;
}

Camera &Camera::Pitch(float angle)
{
    Affine R = rotate(angle, right);
    up = R * up;
    back = R * back;

    return *this;
}

Camera &Camera::Roll(float angle)
{
    Affine R = rotate(angle, back);
    right = R * right;
    up = R * up;

    return *this;
}
