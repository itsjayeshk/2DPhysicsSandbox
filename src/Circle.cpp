#include "Circle.h"

Circle::Circle(const Vector2& startPosition, float circleRadius)
    : PhysicsObject(startPosition, 1.0f), radius(circleRadius), shape(circleRadius)
{
    shape.setOrigin({radius, radius});
    shape.setFillColor(sf::Color(80, 175, 255));
    shape.setOutlineThickness(2.0f);
}

void Circle::draw(sf::RenderWindow& window) const
{
    sf::CircleShape drawable = shape;
    drawable.setPosition({getPosition().getX(), getPosition().getY()});
    drawable.setOutlineColor(selected ? sf::Color::White : sf::Color(30, 85, 130));
    window.draw(drawable);
}

bool Circle::containsPoint(const Vector2& point) const
{
    Vector2 distance = point - getPosition();
    float lengthSquared = distance.getX() * distance.getX() + distance.getY() * distance.getY();
    return lengthSquared <= radius * radius;
}

float Circle::getBottom() const
{
    return getPosition().getY() + radius;
}

void Circle::fixGroundCollision(float groundY)
{
    Vector2 position = getPosition();
    position.setY(groundY - radius);
    setPosition(position);
}
