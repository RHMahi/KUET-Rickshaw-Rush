#ifndef CAMPUS_H
#define CAMPUS_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace KUET {

// Campus building geometry shared by the game world and the custom-mission builder
struct CampusBuilding {
    std::string name;
    float x, y, width, height;
    sf::Color color;
    sf::Color roofColor;
    bool hasChimney;
};

// Road network segment (must match the roads drawn in the render loop)
struct CampusRoad {
    float x, y, width, height;
    bool isVertical;
};

// Single source of truth for the KUET campus map layout
std::vector<CampusBuilding> buildCampusBuildings();
std::vector<CampusRoad>     buildCampusRoads();

}

#endif