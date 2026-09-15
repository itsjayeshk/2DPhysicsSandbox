#include "Vector2.h"

Vector2::Vector2(float xValue, float yValue)
    : x(xValue), y(yValue)
{
}

float Vector2::getX() const
{
    return x;
}

float Vector2::getY() const
{
    return y;
}

void Vector2::setX(float value)
{
    x = value;
}

void Vector2::setY(float value)
{
    y = value;
}

Vector2 Vector2::operator+(const Vector2& other) const
{
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const
{
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(float scalar) const
{
    return Vector2(x * scalar, y * scalar);
}

Vector2& Vector2::operator+=(const Vector2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2& Vector2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}
