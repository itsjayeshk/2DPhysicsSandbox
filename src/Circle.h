#pragma once

#include "PhysicsObject.h"

class Circle : public PhysicsObject
{
private:
    float radius;
    sf::CircleShape shape;

public:
    Circle(const Vector2& startPosition, float circleRadius = 28.0f);

    void draw(sf::RenderWindow& window) const override;
    bool containsPoint(const Vector2& point) const override;
    float getBottom() const override;
    void fixGroundCollision(float groundY) override;
};
