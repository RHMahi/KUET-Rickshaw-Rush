#include "../include/Environment.h"
#include "../include/Constants.h"
#include <cstdlib>
#include <ctime>

using namespace KUET;

Environment::Environment() : GameObject(0, 0), currentTime(0) {
    
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    
    // 1. Sky & Celestial (Sun/Moon)
    sky.setSize(sf::Vector2f(10000.0f, GameSettings::GROUND_Y));
    sky.setPosition(sf::Vector2f(-1000.0f, 0.0f));
    
    celestialBody.setRadius(40.0f);
    celestialBody.setPosition(sf::Vector2f(500.0f, 100.0f));

    // Stars for night mode
    for(int i = 0; i < 150; i++) {
        sf::CircleShape star(1.5f);
        int alpha = 50 + (rand() % 205);  
        star.setFillColor(sf::Color(255, 255, 255, alpha));
        star.setPosition(sf::Vector2f((rand() % 10000) - 1000, rand() % 600));
        stars.push_back(star);
    }

    // 2. Nature: Grass & Pond (only these, roads & trees removed to avoid overlap)
    grassTop.setSize(sf::Vector2f(10000.0f, 50.0f));
    grassTop.setPosition(sf::Vector2f(-1000.0f, GameSettings::GROUND_Y - 50.0f));
    grassTop.setFillColor(sf::Color(34, 139, 34)); // Grass Green

    pond.setSize(sf::Vector2f(800.0f, 35.0f));
    pond.setPosition(sf::Vector2f(1500.0f, GameSettings::GROUND_Y - 45.0f));
    pond.setFillColor(sf::Color(30, 144, 255, 200)); // Transparent Water
    
    setTime(0);
}

void Environment::setTime(int timeMode) {
    currentTime = timeMode;
    if (timeMode == 0) {  // MORNING
        sky.setFillColor(sf::Color(135, 206, 235));  // Sky Blue
        celestialBody.setFillColor(sf::Color(255, 223, 0)); // Yellow Sun
    } else if (timeMode == 1) {  // AFTERNOON
        sky.setFillColor(sf::Color(255, 140, 0));    // Orange
        celestialBody.setFillColor(sf::Color(255, 69, 0)); // Orange-red Sun
    } else if (timeMode == 2) {  // NIGHT
        sky.setFillColor(sf::Color(10, 10, 40));     // Dark Blue
        celestialBody.setFillColor(sf::Color(240, 240, 255)); // White Moon
    }
}

void Environment::update(float dt) {
   
}

void Environment::draw(sf::RenderWindow& window) {
    // 1. Draw Sky
    window.draw(sky);
    
    // 2. Draw Stars (only at night)
    if (currentTime == 2) {
        for(auto& star : stars) {
            window.draw(star);
        }
    }
    
    // 3. Draw Sun/Moon
    window.draw(celestialBody);
    
    // 4. Draw Grass top (green strip above road)
    window.draw(grassTop);
    
    // 5. Draw Pond
    window.draw(pond);
    
    
}