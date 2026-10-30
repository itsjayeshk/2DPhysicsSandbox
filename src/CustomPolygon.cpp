#include "CustomPolygon.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>

namespace
{
constexpr float Epsilon = 0.001f;
constexpr float MinimumArea = 300.0f;

float cross(const Vector2& a, const Vector2& b, const Vector2& c)
{
    return (b.getX() - a.getX()) * (c.getY() - a.getY()) -
           (b.getY() - a.getY()) * (c.getX() - a.getX());
}

float signedArea(const std::vector<Vector2>& points)
{
    float twiceArea = 0.0f;
    for (std::size_t i = 0; i < points.size(); ++i)
    {
        const Vector2& a = points[i];
        const Vector2& b = points[(i + 1) % points.size()];
        twiceArea += a.getX() * b.getY() - b.getX() * a.getY();
    }
    return twiceArea * 0.5f;
}

bool onSegment(const Vector2& a, const Vector2& b, const Vector2& point)
{
    return point.getX() >= std::min(a.getX(), b.getX()) - Epsilon &&
           point.getX() <= std::max(a.getX(), b.getX()) + Epsilon &&
           point.getY() >= std::min(a.getY(), b.getY()) - Epsilon &&
           point.getY() <= std::max(a.getY(), b.getY()) + Epsilon;
}

bool segmentsIntersect(const Vector2& a, const Vector2& b,
                       const Vector2& c, const Vector2& d)
{
    const float abC = cross(a, b, c);
    const float abD = cross(a, b, d);
    const float cdA = cross(c, d, a);
    const float cdB = cross(c, d, b);

    if (((abC > Epsilon && abD < -Epsilon) || (abC < -Epsilon && abD > Epsilon)) &&
        ((cdA > Epsilon && cdB < -Epsilon) || (cdA < -Epsilon && cdB > Epsilon)))
    {
        return true;
    }

    return (std::abs(abC) <= Epsilon && onSegment(a, b, c)) ||
           (std::abs(abD) <= Epsilon && onSegment(a, b, d)) ||
           (std::abs(cdA) <= Epsilon && onSegment(c, d, a)) ||
           (std::abs(cdB) <= Epsilon && onSegment(c, d, b));
}

bool pointInTriangle(const Vector2& point, const Vector2& a,
                     const Vector2& b, const Vector2& c)
{
    const float ab = cross(a, b, point);
    const float bc = cross(b, c, point);
    const float ca = cross(c, a, point);
    const bool hasNegative = ab < -Epsilon || bc < -Epsilon || ca < -Epsilon;
    const bool hasPositive = ab > Epsilon || bc > Epsilon || ca > Epsilon;
    return !(hasNegative && hasPositive);
}
}

CustomPolygon::CustomPolygon(const std::vector<Vector2>& worldVertices)
    : PhysicsObject(Vector2(), 2.0f)
{
    Vector2 center;
    for (const Vector2& vertex : worldVertices)
    {
        center += vertex;
    }
    center = center * (1.0f / static_cast<float>(worldVertices.size()));
    setPosition(center);

    localVertices.reserve(worldVertices.size());
    for (const Vector2& vertex : worldVertices)
    {
        localVertices.push_back(vertex - center);
    }
    buildTriangulation(localVertices, triangleIndices);
}

bool CustomPolygon::isValidOutline(const std::vector<Vector2>& vertices)
{
    if (vertices.size() < 3 || vertices.size() > 160 ||
        std::abs(signedArea(vertices)) < MinimumArea)
    {
        return false;
    }

    for (std::size_t i = 0; i < vertices.size(); ++i)
    {
        const std::size_t nextI = (i + 1) % vertices.size();
        const float dx = vertices[nextI].getX() - vertices[i].getX();
        const float dy = vertices[nextI].getY() - vertices[i].getY();
        if (dx * dx + dy * dy < 16.0f)
        {
            return false;
        }

        for (std::size_t j = i + 1; j < vertices.size(); ++j)
        {
            const std::size_t nextJ = (j + 1) % vertices.size();
            if (i == j || nextI == j || nextJ == i)
            {
                continue;
            }
            if (segmentsIntersect(vertices[i], vertices[nextI], vertices[j], vertices[nextJ]))
            {
                return false;
            }
        }
    }

    std::vector<std::size_t> triangles;
    return buildTriangulation(vertices, triangles);
}

bool CustomPolygon::buildTriangulation(const std::vector<Vector2>& vertices,
                                       std::vector<std::size_t>& triangles)
{
    triangles.clear();
    if (vertices.size() < 3)
    {
        return false;
    }

    std::vector<std::size_t> remaining(vertices.size());
    std::iota(remaining.begin(), remaining.end(), 0);
    const bool counterClockwise = signedArea(vertices) > 0.0f;

    while (remaining.size() > 3)
    {
        bool clippedEar = false;
        for (std::size_t i = 0; i < remaining.size(); ++i)
        {
            const std::size_t previous = remaining[(i + remaining.size() - 1) % remaining.size()];
            const std::size_t current = remaining[i];
            const std::size_t next = remaining[(i + 1) % remaining.size()];
            const float turn = cross(vertices[previous], vertices[current], vertices[next]);

            if ((counterClockwise && turn <= Epsilon) || (!counterClockwise && turn >= -Epsilon))
            {
                continue;
            }

            bool containsOtherVertex = false;
            for (const std::size_t candidate : remaining)
            {
                if (candidate == previous || candidate == current || candidate == next)
                {
                    continue;
                }
                if (pointInTriangle(vertices[candidate], vertices[previous],
                                    vertices[current], vertices[next]))
                {
                    containsOtherVertex = true;
                    break;
                }
            }

            if (containsOtherVertex)
            {
                continue;
            }

            triangles.push_back(previous);
            triangles.push_back(current);
            triangles.push_back(next);
            remaining.erase(remaining.begin() + static_cast<std::ptrdiff_t>(i));
            clippedEar = true;
            break;
        }

        if (!clippedEar)
        {
            triangles.clear();
            return false;
        }
    }

    triangles.insert(triangles.end(), remaining.begin(), remaining.end());
    return true;
}

void CustomPolygon::draw(sf::RenderWindow& window) const
{
    const Vector2 position = getPosition();
    const sf::Color fill = selected ? sf::Color(250, 220, 130) : sf::Color(195, 125, 235);

    sf::VertexArray triangles(sf::PrimitiveType::Triangles);
    for (const std::size_t index : triangleIndices)
    {
        const Vector2& local = localVertices[index];
        sf::Vertex vertex;
        vertex.position = {position.getX() + local.getX(), position.getY() + local.getY()};
        vertex.color = fill;
        triangles.append(vertex);
    }
    window.draw(triangles);

    sf::VertexArray outline(sf::PrimitiveType::LineStrip);
    for (const Vector2& local : localVertices)
    {
        sf::Vertex vertex;
        vertex.position = {position.getX() + local.getX(), position.getY() + local.getY()};
        vertex.color = selected ? sf::Color::White : sf::Color(100, 55, 130);
        outline.append(vertex);
    }
    if (!localVertices.empty())
    {
        sf::Vertex closing = outline[0];
        outline.append(closing);
    }
    window.draw(outline);
}

bool CustomPolygon::containsPoint(const Vector2& point) const
{
    const Vector2 position = getPosition();
    const Vector2 localPoint = point - position;
    bool inside = false;

    for (std::size_t i = 0, j = localVertices.size() - 1; i < localVertices.size(); j = i++)
    {
        const Vector2& a = localVertices[i];
        const Vector2& b = localVertices[j];
        const bool crosses = ((a.getY() > localPoint.getY()) != (b.getY() > localPoint.getY())) &&
            (localPoint.getX() < (b.getX() - a.getX()) *
             (localPoint.getY() - a.getY()) / (b.getY() - a.getY()) + a.getX());
        if (crosses)
        {
            inside = !inside;
        }
    }
    return inside;
}

float CustomPolygon::getBottom() const
{
    float lowest = localVertices.front().getY();
    for (const Vector2& vertex : localVertices)
    {
        lowest = std::max(lowest, vertex.getY());
    }
    return getPosition().getY() + lowest;
}

void CustomPolygon::fixGroundCollision(float groundY)
{
    Vector2 position = getPosition();
    position.setY(position.getY() + groundY - getBottom());
    setPosition(position);
}

float CustomPolygon::getCollisionRadius() const
{
    float radiusSquared = 0.0f;
    for (const Vector2& vertex : localVertices)
    {
        radiusSquared = std::max(radiusSquared,
                                 vertex.getX() * vertex.getX() + vertex.getY() * vertex.getY());
    }
    return std::sqrt(radiusSquared);
}
