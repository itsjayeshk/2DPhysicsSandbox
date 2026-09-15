#pragma once

#include "Vector2.h"

class PhysicsBody
{
private:
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float mass;

public:
    PhysicsBody(const Vector2& startPosition = Vector2(), float bodyMass = 1.0f);

    Vector2 getPosition() const;
    Vector2 getVelocity() const;
    Vector2 getAcceleration() const;
    float getMass() const;

    void setPosition(const Vector2& newPosition);
    void setVelocity(const Vector2& newVelocity);
    void setAcceleration(const Vector2& newAcceleration);
    void setMass(float newMass);

    void update(float deltaTime);
};
