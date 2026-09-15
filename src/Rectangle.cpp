#include "Rectangle.h"

Rectangle::Rectangle(const Vector2& startPosition, const sf::Vector2f& rectangleSize)
    : PhysicsObject(startPosition, 1.4f), size(rectangleSize), shape(rectangleSize)
{
    shape.setOrigin({size.x / 2.0f, size.y / 2.0f});
    shape.setFillColor(sf::Color(255, 185, 80));
    shape.setOutlineThickness(2.0f);
}

void Rectangle::draw(sf::RenderWindow& window) const
{
    sf::RectangleShape drawable = shape;
    drawable.setPosition({getPosition().getX(), getPosition().getY()});
    drawable.setOutlineColor(selected ? sf::Color::White : sf::Color(130, 80, 25));
    window.draw(drawable);
}

bool Rectangle::containsPoint(const Vector2& point) const
{
    Vector2 position = getPosition();
    float left = position.getX() - size.x / 2.0f;
    float right = position.getX() + size.x / 2.0f;
    float top = position.getY() - size.y / 2.0f;
    float bottom = position.getY() + size.y / 2.0f;

    return point.getX() >= left && point.getX() <= right &&
           point.getY() >= top && point.getY() <= bottom;
}

float Rectangle::getBottom() const
{
    return getPosition().getY() + size.y / 2.0f;
}

void Rectangle::fixGroundCollision(float groundY)
{
    Vector2 position = getPosition();
    position.setY(groundY - size.y / 2.0f);
    setPosition(position);
}
