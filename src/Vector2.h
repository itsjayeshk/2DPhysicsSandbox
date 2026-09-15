#pragma once

class Vector2
{
private:
    float x;
    float y;

public:
    Vector2(float xValue = 0.0f, float yValue = 0.0f);

    float getX() const;
    float getY() const;
    void setX(float value);
    void setY(float value);

    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;

    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(float scalar);
};
