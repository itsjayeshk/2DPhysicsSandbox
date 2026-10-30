#pragma once

#include <vector>

#include "PhysicsObject.h"

// A rigid, user-drawn simple polygon. Concave outlines are supported for drawing
// by splitting their fill into triangles; physics remains ground-only.
class CustomPolygon : public PhysicsObject
{
private:
    std::vector<Vector2> localVertices;
    std::vector<std::size_t> triangleIndices;

    static bool buildTriangulation(const std::vector<Vector2>& vertices,
                                   std::vector<std::size_t>& triangles);

public:
    explicit CustomPolygon(const std::vector<Vector2>& worldVertices);

    static bool isValidOutline(const std::vector<Vector2>& vertices);

    void draw(sf::RenderWindow& window) const override;
    bool containsPoint(const Vector2& point) const override;
    float getBottom() const override;
    void fixGroundCollision(float groundY) override;
    float getCollisionRadius() const override;
};
