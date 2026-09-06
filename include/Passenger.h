#ifndef PASSENGER_H
#define PASSENGER_H

#include "GameObject.h"
#include "Rickshaw.h"
#include <string>
#include <cmath>

namespace KUET {
    class Passenger : public GameObject {
    private:
        sf::CircleShape shape;      // collision + base body
        sf::CircleShape head;       // face
        sf::RectangleShape torso;   // body
        sf::RectangleShape armL, armR;  // waving arms
        sf::RectangleShape legL, legR;  // legs
        sf::CircleShape shadow;
        std::string destination;
        std::string passengerName;
        int fare;
        bool isPickedUp;
        int missionId;
        bool pickupPrompt;
        float animTimer = 0.0f;
        
    public:
        Passenger(float sx, float sy, std::string name, std::string dest, int amount);
        ~Passenger() = default;
        
        void update(float dt) override;
        void update(Rickshaw* player);
        void draw(sf::RenderWindow& window) override;
        void pickUp();
        
        bool isPicked() const;
        bool isNear() const { return pickupPrompt; }
        void reset();
        int getFare() const;
        std::string getDestination() const;
        std::string getPassengerName() const;
        void setMissionId(int id);
        int getMissionId() const;
        
        sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    };
}

#endif