#include "Triangle.h"

Triangle::Triangle(const Vector2& startPosition, float triangleSize)
    : PhysicsObject(startPosition, 1.2f), size(triangleSize), shape(triangleSize, 3)
{
    shape.setOrigin({size, size});
    shape.setRotation(sf::degrees(30.0f));
    shape.setFillColor(sf::Color(120, 225, 140));
    shape.setOutlineThickness(2.0f);
}

void Triangle::draw(sf::RenderWindow& window) const
{
    sf::CircleShape drawable = shape;
    drawable.setPosition({getPosition().getX(), getPosition().getY()});
    drawable.setOutlineColor(selected ? sf::Color::White : sf::Color(35, 115, 55));
    window.draw(drawable);
}

bool Triangle::containsPoint(const Vector2& point) const
{
    Vector2 distance = point - getPosition();
    float lengthSquared = distance.getX() * distance.getX() + distance.getY() * distance.getY();
    return lengthSquared <= size * size;
}

float Triangle::getBottom() const
{
    return getPosition().getY() + size;
}

void Triangle::fixGroundCollision(float groundY)
{
    Vector2 position = getPosition();
    position.setY(groundY - size);
    setPosition(position);
}
