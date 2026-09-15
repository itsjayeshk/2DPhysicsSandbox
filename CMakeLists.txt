cmake_minimum_required(VERSION 3.20)

project(PhysicsSandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.1 COMPONENTS Graphics Window System REQUIRED)

add_executable(PhysicsSandbox
    src/main.cpp
    src/Vector2.cpp
    src/PhysicsBody.cpp
    src/PhysicsObject.cpp
    src/Circle.cpp
    src/Rectangle.cpp
    src/Triangle.cpp
    src/Game.cpp
)

target_link_libraries(PhysicsSandbox PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
