#include "Game.h"

#include <algorithm>
#include <optional>
#include <sstream>

#include "Circle.h"
#include "Rectangle.h"
#include "Triangle.h"

Game::Game()
    : window(sf::VideoMode({900, 650}), "2D Physics Sandbox - OOP Project"),
      selectedObject(nullptr),
      dragOffset(0.0f, 0.0f),
      dragging(false),
      paused(false),
      fontLoaded(false),
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
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                selectObjectAt(Vector2(static_cast<float>(mouse->position.x),
                                       static_cast<float>(mouse->position.y)));
            }
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseMoved>())
        {
            if (dragging && selectedObject != nullptr)
            {
                dragSelectedObject(Vector2(static_cast<float>(mouse->position.x),
                                           static_cast<float>(mouse->position.y)));
            }
        }

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouse->button == sf::Mouse::Button::Left)
            {
                dragging = false;
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
            object->update(deltaTime, gravity, groundY);
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

void Game::reset()
{
    objects.clear();
    selectedObject = nullptr;
    dragging = false;
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
    drawTextLine("1 Circle   2 Rectangle   3 Triangle", 15.0f, 12.0f);
    drawTextLine("Mouse drag: select/move   Delete: remove selected", 15.0f, 34.0f);
    drawTextLine("Space: pause/resume   R: reset", 15.0f, 56.0f);

    std::ostringstream status;
    status << "Objects: " << objects.size();
    status << "   State: " << (paused ? "Paused" : "Running");
    drawTextLine(status.str(), 15.0f, 84.0f);
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
