#ifndef MINIMAP_H
#define MINIMAP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace KUET {

struct MinimapBuilding {
    std::string name;
    float worldX, worldY;
    float width, height;
    sf::Color color;
};

class Minimap {
private:
    sf::RectangleShape minimapBg;
    sf::RectangleShape playerDot;
    std::vector<sf::RectangleShape> buildingRects;
    sf::Text label;                     
    float scale;
    float offsetX, offsetY;
    float worldWidth, worldHeight;
    float mapWidth, mapHeight;
    
public:
    // Member initializer list
    Minimap(float worldW, float worldH, float screenW, float screenH, sf::Font& fnt)
        : label(fnt, "MAP", 10)          
    {
        worldWidth = worldW;
        worldHeight = worldH;
        mapWidth = 180;
        mapHeight = 135;
        scale = mapWidth / worldWidth;
        offsetX = screenW - mapWidth - 10;
        offsetY = 10;
        
        minimapBg.setSize(sf::Vector2f(mapWidth, mapHeight));
        minimapBg.setFillColor(sf::Color(0, 0, 0, 180));
        minimapBg.setOutlineThickness(1);
        minimapBg.setOutlineColor(sf::Color::White);
        minimapBg.setPosition(sf::Vector2f(offsetX, offsetY));
        
        playerDot.setSize(sf::Vector2f(4, 4));
        playerDot.setFillColor(sf::Color::Red);
        
        label.setFillColor(sf::Color::White);
        label.setPosition(sf::Vector2f(offsetX + 5, offsetY + 5));
    }
    
    void updateBuildings(const std::vector<MinimapBuilding>& buildings) {
        buildingRects.clear();
        for (const auto& b : buildings) {
            sf::RectangleShape rect(sf::Vector2f(b.width * scale, b.height * scale));
            rect.setFillColor(b.color);
            rect.setPosition(sf::Vector2f(offsetX + b.worldX * scale, offsetY + b.worldY * scale));
            buildingRects.push_back(rect);
        }
    }
    
    void draw(sf::RenderWindow& window, float playerX, float playerY, float playerAngle = 0.0f) {
        window.draw(minimapBg);
        for (auto& rect : buildingRects) {
            window.draw(rect);
        }
        
        float px = offsetX + playerX * scale;
        float py = offsetY + playerY * scale;
        playerDot.setPosition(sf::Vector2f(px - 2, py - 2));
        window.draw(playerDot);
        
        sf::CircleShape directionTriangle(4, 3);
        directionTriangle.setFillColor(sf::Color::Yellow);
        directionTriangle.setRotation(sf::degrees(playerAngle + 90));
        directionTriangle.setPosition(sf::Vector2f(px - 2, py - 2));
        window.draw(directionTriangle);
        
        window.draw(label);
    }
};

} // namespace KUET

#endif