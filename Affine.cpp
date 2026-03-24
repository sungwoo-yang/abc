#include "Affine.h"

Hcoord::Hcoord(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W)
{
}

Point::Point(float X, float Y, float Z) : Hcoord(X, Y, Z, 1.0f)
{
}

Vector::Vector(float X, float Y, float Z) : Hcoord(X, Y, Z, 0.0f)
{
}

Affine::Affine(void)
{
    row[0] = Hcoord(0, 0, 0, 0);
    row[1] = Hcoord(0, 0, 0, 0);
    row[2] = Hcoord(0, 0, 0, 0);
    row[3] = Hcoord(0, 0, 0, 1);
}

Affine::Affine(const Vector &Lx, const Vector &Ly, const Vector &Lz, const Point &D)
{
    row[0] = Hcoord(Lx.x, Ly.x, Lz.x, D.x);
    row[1] = Hcoord(Lx.y, Ly.y, Lz.y, D.y);
    row[2] = Hcoord(Lx.z, Ly.z, Lz.z, D.z);
    row[3] = Hcoord(Lx.w, Ly.w, Lz.w, D.w);
}

Hcoord operator+(const Hcoord &u, const Hcoord &v)
{
    return Hcoord(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

Hcoord operator-(const Hcoord &u, const Hcoord &v)
{
    return Hcoord(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

Hcoord operator-(const Hcoord &v)
{
    return Hcoord(-v.x, -v.y, -v.z, -v.w);
}

Hcoord operator*(float r, const Hcoord &v)
{
    return Hcoord(v.x * r, v.y * r, v.z * r, v.w * r);
}

Hcoord operator*(const Matrix &A, const Hcoord &v)
{
    Hcoord result(0, 0, 0, 0);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i] += A[i][j] * v[j];
        }
    }

    return result;
}

Matrix operator*(const Matrix &A, const Matrix &B)
{
    Matrix result;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

float dot(const Vector &u, const Vector &v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

float abs(const Vector &v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector normalize(const Vector &v)
{
    float abs_v = abs(v);
    return Vector(v.x / abs_v, v.y / abs_v, v.z / abs_v);
}

Vector cross(const Vector &u, const Vector &v)
{
    return Vector(u.y * v.z - u.z * v.y, -(u.x * v.z - u.z * v.x), u.x * v.y - u.y * v.x);
}

Affine rotate(float t, const Vector &v)
{
    Vector u = normalize(v);
    float c = std::cos(t);
    float s = std::sin(t);
    float omc = 1.0f - c;

    Vector Lx(c + omc * u.x * u.x, omc * u.y * u.x + s * u.z, omc * u.z * u.x - s * u.y);
    Vector Ly(omc * u.x * u.y - s * u.z, c + omc * u.y * u.y, omc * u.z * u.y + s * u.x);
    Vector Lz(omc * u.x * u.z + s * u.y, omc * u.y * u.z - s * u.x, c + omc * u.z * u.z);

    return Affine(Lx, Ly, Lz, Point(0, 0, 0));
}

Affine translate(const Vector &v)
{
    Vector Lx(1, 0, 0);
    Vector Ly(0, 1, 0);
    Vector Lz(0, 0, 1);
    Point t(v.x, v.y, v.z);

    return Affine(Lx, Ly, Lz, t);
}

Affine scale(float r)
{
    Vector Lx(r, 0, 0);
    Vector Ly(0, r, 0);
    Vector Lz(0, 0, r);
    Point p(0, 0, 0);

    return Affine(Lx, Ly, Lz, p);
}

Affine scale(float rx, float ry, float rz)
{
    Vector Lx(rx, 0, 0);
    Vector Ly(0, ry, 0);
    Vector Lz(0, 0, rz);
    Point p(0, 0, 0);

    return Affine(Lx, Ly, Lz, p);
}

Affine inverse(const Affine &A)
{
    Vector Lx(A[0][0], A[1][0], A[2][0]);
    Vector Ly(A[0][1], A[1][1], A[2][1]);
    Vector Lz(A[0][2], A[1][2], A[2][2]);

    float det_L = dot(Lx, cross(Ly, Lz));
    float inv_det = 1.0f / det_L;

    Vector Row0 = inv_det * cross(Ly, Lz);
    Vector Row1 = inv_det * cross(Lz, Lx);
    Vector Row2 = inv_det * cross(Lx, Ly);

    Vector Inverse_Lx(Row0.x, Row1.x, Row2.x);
    Vector Inverse_Ly(Row0.y, Row1.y, Row2.y);
    Vector Inverse_Lz(Row0.z, Row1.z, Row2.z);

    Vector v(A[0][3], A[1][3], A[2][3]);

    Point D(-dot(Row0, v), -dot(Row1, v), -dot(Row2, v));

    return Affine(Inverse_Lx, Inverse_Ly, Inverse_Lz, D);
}
