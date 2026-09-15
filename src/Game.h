#pragma once

#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "PhysicsObject.h"

class Game
{
private:
    sf::RenderWindow window;
    sf::Font font;
    std::vector<std::unique_ptr<PhysicsObject>> objects;

    PhysicsObject* selectedObject;
    Vector2 dragOffset;
    bool dragging;
    bool paused;
    bool fontLoaded;

    const unsigned int windowWidth;
    const unsigned int windowHeight;
    const float groundY;
    const float gravity;

    void handleEvents();
    void update(float deltaTime);
    void render();

    void spawnCircle();
    void spawnRectangle();
    void spawnTriangle();
    void reset();
    void deleteSelectedObject();

    void selectObjectAt(const Vector2& mousePosition);
    void dragSelectedObject(const Vector2& mousePosition);

    void drawGrid();
    void drawGround();
    void drawText();
    void drawTextLine(const std::string& text, float x, float y, unsigned int size = 16);

public:
    Game();
    void run();
};
