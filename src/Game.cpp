#include "Game.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <sstream>

#include "Circle.h"
#include "CustomPolygon.h"
#include "Rectangle.h"
#include "Triangle.h"

Game::Game()
    : window(sf::VideoMode({900, 650}), "2D Physics Sandbox - OOP Project"),
      selectedObject(nullptr),
      dragOffset(0.0f, 0.0f),
      dragging(false),
      paused(false),
      fontLoaded(false),
      drawingShape(false),
      drawingStroke(false),
      gravityEnabled(true),
      airResistanceEnabled(false),
      frictionEnabled(true),
      windowWidth(900),
      windowHeight(650),
      groundY(570.0f),
      gravity(900.0f)
{
    window.setFramerateLimit(60);

    fontLoaded = font.openFromFile("assets/arial.ttf") ||
                 font.openFromFile("C:/Windows/Fonts/arial.ttf");
}

void Game::run()
{
    sf::Clock clock;

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

        handleEvents();

        if (!paused)
        {
            update(deltaTime);
        }

        render();
    }
}

void Game::handleEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Num1)
            {
                spawnCircle();
            }
            else if (key->code == sf::Keyboard::Key::Num2)
            {
                spawnRectangle();
            }
            else if (key->code == sf::Keyboard::Key::Num3)
            {
                spawnTriangle();
            }
            else if (key->code == sf::Keyboard::Key::R)
            {
                reset();
            }
            else if (key->code == sf::Keyboard::Key::Space)
            {
                paused = !paused;
            }
            else if (key->code == sf::Keyboard::Key::Delete)
            {
                deleteSelectedObject();
            }
            else if (key->code == sf::Keyboard::Key::G)
            {
                gravityEnabled = !gravityEnabled;
            }
            else if (key->code == sf::Keyboard::Key::A)
            {
                airResistanceEnabled = !airResistanceEnabled;
            }
            else if (key->code == sf::Keyboard::Key::F)
            {
                frictionEnabled = !frictionEnabled;
            }
            else if (key->code == sf::Keyboard::Key::E)
            {
                pushSelectedObject();
            }
            else if (key->code == sf::Keyboard::Key::D)
            {
                drawingShape = !drawingShape;
                drawingStroke = false;
                drawingPoints.clear();
                selectedObject = nullptr;
                for (const auto& object : objects)
                {
                    object->setSelected(false);
                }
                dragging = false;
                drawingMessage = drawingShape ? "Draw mode: drag the mouse to outline a shape" : "Draw mode off";
            }
            else if (key->code == sf::Keyboard::Key::Escape && drawingShape)
            {
                cancelDrawingShape();
            }
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2f worldPosition = window.mapPixelToCoords(mouse->position);
                const Vector2 point(worldPosition.x, worldPosition.y);
                if (drawingShape)
                {
                    drawingStroke = true;
                    drawingPoints.clear();
                    addDrawingPoint(point);
                }
                else
                {
                    selectObjectAt(point);
                }
            }
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
        {
            const sf::Vector2f worldPosition = window.mapPixelToCoords(mouse->position);
            const Vector2 point(worldPosition.x, worldPosition.y);
            if (drawingShape && drawingStroke)
            {
                addDrawingPoint(point);
            }
            else if (dragging && selectedObject != nullptr)
            {
                dragSelectedObject(point);
            }
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                dragging = false;
                if (drawingShape && drawingStroke)
                {
                    const sf::Vector2f worldPosition = window.mapPixelToCoords(mouse->position);
                    addDrawingPoint(Vector2(worldPosition.x, worldPosition.y));
                    drawingStroke = false;
                    finishDrawingShape();
                }
            }
        }
    }
}

void Game::update(float deltaTime)
{
    for (const auto& object : objects)
    {
        if (object.get() != selectedObject || !dragging)
        {
            object->update(deltaTime, gravityEnabled ? gravity : 0.0f, groundY);

            Vector2 velocity = object->getVelocity();
            const bool touchingGround = object->getBottom() >= groundY - 0.5f;
            if (airResistanceEnabled && !touchingGround)
            {
                // Linear drag: velocity is reduced in proportion to elapsed time.
                const float drag = std::max(0.0f, 1.0f - 0.35f * deltaTime);
                velocity *= drag;
            }
            if (frictionEnabled && touchingGround)
            {
                // Ground friction slows horizontal motion while an object rests on the floor.
                const float friction = std::max(0.0f, 1.0f - 5.0f * deltaTime);
                velocity.setX(velocity.getX() * friction);
                if (std::abs(velocity.getX()) < 8.0f)
                {
                    velocity.setX(0.0f);
                }
            }
            object->setVelocity(velocity);
        }
    }

    // Resolve every pair, including objects currently being dragged.
    for (std::size_t i = 0; i < objects.size(); ++i)
    {
        for (std::size_t j = i + 1; j < objects.size(); ++j)
        {
            objects[i]->resolveCollision(*objects[j]);
        }
    }
}

void Game::render()
{
    window.clear(sf::Color(18, 22, 28));

    drawGrid();
    drawGround();

    for (const auto& object : objects)
    {
        object->draw(window);
    }

    drawShapePreview();
    drawText();

    window.display();
}

void Game::spawnCircle()
{
    objects.push_back(std::make_unique<Circle>(Vector2(180.0f, 80.0f)));
}

void Game::spawnRectangle()
{
    objects.push_back(std::make_unique<Rectangle>(Vector2(450.0f, 80.0f)));
}

void Game::spawnTriangle()
{
    objects.push_back(std::make_unique<Triangle>(Vector2(720.0f, 80.0f)));
}

void Game::addDrawingPoint(const Vector2& point)
{
    if (point.getX() < 0.0f || point.getY() < 0.0f ||
        point.getX() > static_cast<float>(windowWidth) ||
        point.getY() >= groundY - 4.0f || drawingPoints.size() >= 160)
    {
        return;
    }

    if (drawingPoints.empty())
    {
        drawingPoints.push_back(point);
        return;
    }

    const Vector2 difference = point - drawingPoints.back();
    if (difference.getX() * difference.getX() + difference.getY() * difference.getY() >= 25.0f)
    {
        drawingPoints.push_back(point);
    }
}

void Game::finishDrawingShape()
{
    if (drawingPoints.size() > 2)
    {
        const Vector2 closingDifference = drawingPoints.front() - drawingPoints.back();
        if (closingDifference.getX() * closingDifference.getX() +
            closingDifference.getY() * closingDifference.getY() < 25.0f)
        {
            drawingPoints.pop_back();
        }
    }

    if (!CustomPolygon::isValidOutline(drawingPoints))
    {
        drawingMessage = "Shape rejected: draw a larger, simple outline without crossing edges";
        drawingPoints.clear();
        return;
    }

    objects.push_back(std::make_unique<CustomPolygon>(drawingPoints));
    drawingMessage = "Custom shape added";
    drawingPoints.clear();
}

void Game::cancelDrawingShape()
{
    drawingShape = false;
    drawingStroke = false;
    drawingPoints.clear();
    drawingMessage = "Drawing cancelled";
}

void Game::reset()
{
    objects.clear();
    selectedObject = nullptr;
    dragging = false;
    drawingStroke = false;
    drawingPoints.clear();
}

void Game::deleteSelectedObject()
{
    if (selectedObject == nullptr)
    {
        return;
    }

    objects.erase(
        std::remove_if(objects.begin(), objects.end(),
                       [this](const std::unique_ptr<PhysicsObject>& object)
                       {
                           return object.get() == selectedObject;
                       }),
        objects.end());

    selectedObject = nullptr;
    dragging = false;
}

void Game::pushSelectedObject()
{
    if (selectedObject == nullptr)
    {
        return;
    }

    // Give every shape the same impulse; lighter objects gain more speed (J = m * change in velocity).
    Vector2 velocity = selectedObject->getVelocity();
    velocity += Vector2(420.0f / selectedObject->getMass(), -260.0f / selectedObject->getMass());
    selectedObject->setVelocity(velocity);
}

void Game::selectObjectAt(const Vector2& mousePosition)
{
    selectedObject = nullptr;

    for (const auto& object : objects)
    {
        object->setSelected(false);
    }

    for (auto iterator = objects.rbegin(); iterator != objects.rend(); ++iterator)
    {
        if ((*iterator)->containsPoint(mousePosition))
        {
            selectedObject = iterator->get();
            selectedObject->setSelected(true);
            dragOffset = selectedObject->getPosition() - mousePosition;
            selectedObject->stop();
            dragging = true;
            return;
        }
    }

    dragging = false;
}

void Game::dragSelectedObject(const Vector2& mousePosition)
{
    selectedObject->setPosition(mousePosition + dragOffset);
    selectedObject->stop();
}

void Game::drawGrid()
{
    sf::VertexArray lines(sf::PrimitiveType::Lines);
    const float spacing = 50.0f;
    const sf::Color gridColor(35, 42, 52);

    for (float x = 0.0f; x <= static_cast<float>(windowWidth); x += spacing)
    {
        sf::Vertex top;
        top.position = {x, 0.0f};
        top.color = gridColor;

        sf::Vertex bottom;
        bottom.position = {x, static_cast<float>(windowHeight)};
        bottom.color = gridColor;

        lines.append(top);
        lines.append(bottom);
    }

    for (float y = 0.0f; y <= static_cast<float>(windowHeight); y += spacing)
    {
        sf::Vertex left;
        left.position = {0.0f, y};
        left.color = gridColor;

        sf::Vertex right;
        right.position = {static_cast<float>(windowWidth), y};
        right.color = gridColor;

        lines.append(left);
        lines.append(right);
    }

    window.draw(lines);
}

void Game::drawGround()
{
    sf::RectangleShape ground({static_cast<float>(windowWidth), 8.0f});
    ground.setPosition({0.0f, groundY});
    ground.setFillColor(sf::Color(220, 220, 220));
    window.draw(ground);
}

void Game::drawText()
{
    drawTextLine("1 Circle   2 Rectangle   3 Triangle   D Draw custom shape", 15.0f, 12.0f);
    drawTextLine(drawingShape ? "Drag to draw   Esc: cancel" : "Mouse drag: select/move   Delete: remove selected", 15.0f, 34.0f);
    drawTextLine("Space: pause   R: reset   G: gravity   A: air resistance   F: ground friction   E: push selected", 15.0f, 56.0f, 14);

    std::ostringstream status;
    status << "Objects: " << objects.size();
    status << "   State: " << (paused ? "Paused" : "Running");
    drawTextLine(status.str(), 15.0f, 82.0f);
    std::ostringstream concepts;
    concepts << "Gravity: " << (gravityEnabled ? "ON" : "OFF")
             << "   Air drag: " << (airResistanceEnabled ? "ON" : "OFF")
             << "   Ground friction: " << (frictionEnabled ? "ON" : "OFF");
    drawTextLine(concepts.str(), 15.0f, 104.0f, 14);
    if (!drawingMessage.empty())
    {
        drawTextLine(drawingMessage, 15.0f, 126.0f, 14);
    }
}

void Game::drawShapePreview()
{
    if (!drawingShape || drawingPoints.empty())
    {
        return;
    }

    sf::VertexArray preview(sf::PrimitiveType::LineStrip);
    for (const Vector2& point : drawingPoints)
    {
        sf::Vertex vertex;
        vertex.position = {point.getX(), point.getY()};
        vertex.color = sf::Color(255, 245, 120);
        preview.append(vertex);
    }
    window.draw(preview);
}

void Game::drawTextLine(const std::string& text, float x, float y, unsigned int size)
{
    if (!fontLoaded)
    {
        return;
    }

    sf::Text line(font, text, size);
    line.setPosition({x, y});
    line.setFillColor(sf::Color(235, 235, 235));
    window.draw(line);
}
