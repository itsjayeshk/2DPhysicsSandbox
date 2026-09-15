#include "PhysicsObject.h"

#include <cmath>

PhysicsObject::PhysicsObject(const Vector2& startPosition, float mass)
    : body(startPosition, mass), selected(false), bounceFactor(0.55f)
{
}

void PhysicsObject::update(float deltaTime, float gravity, float groundY)
{
    body.setAcceleration(Vector2(0.0f, gravity));
    body.update(deltaTime);

    if (getBottom() >= groundY)
    {
        fixGroundCollision(groundY);

        Vector2 velocity = body.getVelocity();
        velocity.setY(-velocity.getY() * bounceFactor);

        if (std::abs(velocity.getY()) < 35.0f)
        {
            velocity.setY(0.0f);
        }

        body.setVelocity(velocity);
    }
}

Vector2 PhysicsObject::getPosition() const
{
    return body.getPosition();
}

Vector2 PhysicsObject::getVelocity() const
{
    return body.getVelocity();
}

bool PhysicsObject::isSelected() const
{
    return selected;
}

void PhysicsObject::setPosition(const Vector2& newPosition)
{
    body.setPosition(newPosition);
}

void PhysicsObject::setVelocity(const Vector2& newVelocity)
{
    body.setVelocity(newVelocity);
}

void PhysicsObject::setSelected(bool value)
{
    selected = value;
}

void PhysicsObject::stop()
{
    body.setVelocity(Vector2(0.0f, 0.0f));
    body.setAcceleration(Vector2(0.0f, 0.0f));
}
