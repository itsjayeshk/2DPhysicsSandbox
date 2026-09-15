#pragma once

#include "PhysicsObject.h"

class Triangle : public PhysicsObject
{
private:
    float size;
    sf::CircleShape shape;

public:
    Triangle(const Vector2& startPosition, float triangleSize = 34.0f);

    void draw(sf::RenderWindow& window) const override;
    bool containsPoint(const Vector2& point) const override;
    float getBottom() const override;
    void fixGroundCollision(float groundY) override;
};
