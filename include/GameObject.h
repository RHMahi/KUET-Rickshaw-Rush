#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <SFML/Graphics.hpp>

namespace KUET {
    class GameObject {
    protected:
        float x, y;
    public:
        GameObject(float startX, float startY) : x(startX), y(startY) {}

        // Pure virtual functions (Abstraction)
        virtual void update(float dt) = 0;
        virtual void draw(sf::RenderWindow& window) = 0;

        virtual float getX() { return x; }
        virtual float getY() { return y; }
        virtual ~GameObject() {}
    };
}
#endif
