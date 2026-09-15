#pragma once

#include <SFML/Graphics.hpp>

#include "PhysicsBody.h"

class PhysicsObject
{
protected:
    PhysicsBody body;
    bool selected;
    float bounceFactor;

public:
    PhysicsObject(const Vector2& startPosition, float mass);
    virtual ~PhysicsObject() = default;

    virtual void update(float deltaTime, float gravity, float groundY);
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual bool containsPoint(const Vector2& point) const = 0;
    virtual float getBottom() const = 0;
    virtual void fixGroundCollision(float groundY) = 0;

    Vector2 getPosition() const;
    Vector2 getVelocity() const;
    bool isSelected() const;

    void setPosition(const Vector2& newPosition);
    void setVelocity(const Vector2& newVelocity);
    void setSelected(bool value);
    void stop();
};
