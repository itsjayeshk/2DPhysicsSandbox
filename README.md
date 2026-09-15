# 2D Physics Sandbox

This is a simple C++ and SFML project made for a 2nd-year Object-Oriented Programming course. The sandbox lets the user spawn basic shapes, drag them with the mouse, and watch them fall due to gravity and bounce on the ground.

The project is intentionally small and easy to explain. It does not use Box2D or any advanced physics engine.

## Features

- Spawn circles, rectangles, and triangles.
- Gravity pulls every object downward.
- Objects bounce when they hit the ground.
- Select and drag objects using the mouse.
- Delete a selected object.
- Pause/resume and reset the simulation.
- Dark background, simple grid, visible ground, and control text.

## Controls

| Key / Mouse | Action |
| --- | --- |
| `1` | Spawn circle |
| `2` | Spawn rectangle |
| `3` | Spawn triangle |
| `Mouse Left Button` | Select and drag object |
| `Delete` | Delete selected object |
| `Space` | Pause or resume |
| `R` | Reset simulation |

## How to Build and Run

You need:

- C++17 compiler
- CMake
- SFML 3.1

Build:

```bash
cmake -S . -B build
cmake --build build --parallel 1
```

Run:

```bash
./build/PhysicsSandbox
```

On Windows, the executable may be inside a configuration folder:

```bash
./build/Debug/PhysicsSandbox.exe
```

## Project Structure

```text
src/
  main.cpp
  Game.h / Game.cpp
  Vector2.h / Vector2.cpp
  PhysicsBody.h / PhysicsBody.cpp
  PhysicsObject.h / PhysicsObject.cpp
  Circle.h / Circle.cpp
  Rectangle.h / Rectangle.cpp
  Triangle.h / Triangle.cpp
CMakeLists.txt
README.md
```

## Class Hierarchy

```text
PhysicsObject (abstract base class)
|
+-- Circle
+-- Rectangle
+-- Triangle
```

`Game` owns a list of physics objects:

```cpp
std::vector<std::unique_ptr<PhysicsObject>> objects;
```

Because the list stores base-class pointers, the program can call `update()` and `draw()` without knowing the exact shape type.

## OOP Concepts Used

### Encapsulation

Data such as position, velocity, acceleration, and mass is kept inside `PhysicsBody`. These values are accessed using getter and setter methods.

### Inheritance

`Circle`, `Rectangle`, and `Triangle` inherit from `PhysicsObject`.

### Polymorphism

`PhysicsObject` has virtual functions such as:

```cpp
virtual void draw(sf::RenderWindow& window) const = 0;
virtual bool containsPoint(const Vector2& point) const = 0;
```

Each shape gives its own implementation. The `Game` class stores all shapes as `PhysicsObject` pointers and calls these functions polymorphically.

### Abstraction

`PhysicsObject` is an abstract class. It defines common behavior for all objects but cannot be created directly.

### Composition

Each `PhysicsObject` contains a `PhysicsBody` object. This is composition because one class is built using another class.

### Operator Overloading

The `Vector2` class overloads:

```cpp
+
-
*
```

This makes vector calculations simple:

```cpp
velocity += acceleration * deltaTime;
position += velocity * deltaTime;
```

## Basic Physics

The physics is deliberately simple:

```cpp
velocity += acceleration * deltaTime;
position += velocity * deltaTime;
```

Gravity is a downward acceleration:

```cpp
acceleration = Vector2(0.0f, gravity);
```

When an object touches the ground:

```cpp
velocity.y = -velocity.y * bounceFactor;
```

The bounce factor is less than `1`, so every bounce becomes smaller and the object eventually stops.

## Viva Explanation

This project simulates basic falling objects. The base class `PhysicsObject` represents anything that can be updated and drawn. The classes `Circle`, `Rectangle`, and `Triangle` inherit from it and implement their own drawing and mouse selection logic.

The `Game` class runs the SFML window loop. It handles keyboard and mouse input, updates the physics, checks ground collision, and draws everything on the screen.

The physics uses position, velocity, acceleration, and delta time. Gravity increases downward velocity. When the bottom of a shape reaches the ground line, the object is moved back above the ground and its vertical velocity is reversed and reduced to create a bounce.
