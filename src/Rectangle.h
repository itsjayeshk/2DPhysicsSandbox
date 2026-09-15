#pragma once

#include "PhysicsObject.h"

class Rectangle : public PhysicsObject
{
private:
    sf::Vector2f size;
    sf::RectangleShape shape;

public:
    Rectangle(const Vector2& startPosition, const sf::Vector2f& rectangleSize = {70.0f, 45.0f});

    void draw(sf::RenderWindow& window) const override;
    bool containsPoint(const Vector2& point) const override;
    float getBottom() const override;
    void fixGroundCollision(float groundY) override;
};
