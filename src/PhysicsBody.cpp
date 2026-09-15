#include "PhysicsBody.h"

PhysicsBody::PhysicsBody(const Vector2& startPosition, float bodyMass)
    : position(startPosition),
      velocity(0.0f, 0.0f),
      acceleration(0.0f, 0.0f),
      mass(bodyMass)
{
    if (mass <= 0.0f)
    {
        mass = 1.0f;
    }
}

Vector2 PhysicsBody::getPosition() const
{
    return position;
}

Vector2 PhysicsBody::getVelocity() const
{
    return velocity;
}

Vector2 PhysicsBody::getAcceleration() const
{
    return acceleration;
}

float PhysicsBody::getMass() const
{
    return mass;
}

void PhysicsBody::setPosition(const Vector2& newPosition)
{
    position = newPosition;
}

void PhysicsBody::setVelocity(const Vector2& newVelocity)
{
    velocity = newVelocity;
}

void PhysicsBody::setAcceleration(const Vector2& newAcceleration)
{
    acceleration = newAcceleration;
}

void PhysicsBody::setMass(float newMass)
{
    if (newMass > 0.0f)
    {
        mass = newMass;
    }
}

void PhysicsBody::update(float deltaTime)
{
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
}
