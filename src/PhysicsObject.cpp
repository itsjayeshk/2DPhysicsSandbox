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

float PhysicsObject::getMass() const
{
    return body.getMass();
}

void PhysicsObject::resolveCollision(PhysicsObject& other)
{
    const Vector2 difference = other.getPosition() - getPosition();
    const float distanceSquared = difference.getX() * difference.getX() +
                                  difference.getY() * difference.getY();
    // Slightly inset the conservative circular bounds so non-circular shapes
    // do not get pushed apart while their visible outlines are still far apart.
    constexpr float collisionBoundsScale = 0.9f;
    const float minimumDistance =
        (getCollisionRadius() + other.getCollisionRadius()) * collisionBoundsScale;
    if (distanceSquared >= minimumDistance * minimumDistance)
    {
        return;
    }

    const float distance = std::sqrt(distanceSquared);
    const Vector2 normal = distance > 0.001f
        ? difference * (1.0f / distance)
        : Vector2(1.0f, 0.0f);
    const float inverseMassA = 1.0f / getMass();
    const float inverseMassB = 1.0f / other.getMass();
    const float inverseMassTotal = inverseMassA + inverseMassB;
    const float penetration = minimumDistance - distance;

    // Separate the pair in proportion to their inverse mass.
    setPosition(getPosition() - normal * (penetration * inverseMassA / inverseMassTotal));
    other.setPosition(other.getPosition() + normal * (penetration * inverseMassB / inverseMassTotal));

    const Vector2 relativeVelocity = other.getVelocity() - getVelocity();
    const float velocityAlongNormal = relativeVelocity.getX() * normal.getX() +
                                      relativeVelocity.getY() * normal.getY();
    if (velocityAlongNormal < 0.0f)
    {
        constexpr float restitution = 0.25f;
        const float impulse = -(1.0f + restitution) * velocityAlongNormal / inverseMassTotal;
        const Vector2 impulseVector = normal * impulse;
        setVelocity(getVelocity() - impulseVector * inverseMassA);
        other.setVelocity(other.getVelocity() + impulseVector * inverseMassB);
    }
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
