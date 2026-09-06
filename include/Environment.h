#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "GameObject.h"
#include "Constants.h"

class Environment : public KUET::GameObject {
private:
    
    sf::RectangleShape sky;
    sf::CircleShape celestialBody; 
    std::vector<sf::CircleShape> stars; 
    
    
    sf::RectangleShape grassTop;   
    sf::RectangleShape pond;       
    
   
    int currentTime;

public:
    Environment();
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    void setTime(int timeMode);
    int getCurrentTime() const { return currentTime; }
};

#endif