#ifndef RICKSHAW_H
#define RICKSHAW_H

#include "Vehicle.h"
#include <string>
#include <vector>
#include <functional>

namespace KUET {
    class Rickshaw : public Vehicle {
        friend class Mechanic; //mechanic friend to access all vehicles
    private:
        bool hasTurbo;
        bool isMoving;
        float wobbleTimer;
        float wobbleAngle;
        float wheelOffset;
        
        sf::RectangleShape body;
        sf::CircleShape wheel1;
        sf::CircleShape wheel2;
        sf::RectangleShape roof;
        sf::RectangleShape hood;
        sf::CircleShape headlight;

        // Extra realistic detail shapes
        sf::RectangleShape frame;      // main chassis bar
        sf::CircleShape seat;          // driver seat
        sf::RectangleShape handlebar;  // handle bars
        sf::RectangleShape fork;       // front fork
        sf::CircleShape wheelHub1;
        sf::CircleShape wheelHub2;
        sf::RectangleShape pedalCrank; // pedal crank arm
        sf::RectangleShape pedal;      // pedal block
        std::vector<sf::RectangleShape> spokes1; // wheel spokes
        std::vector<sf::RectangleShape> spokes2;
        float pedalAngle;
        float bobPhase;
        
        std::function<void(int)> damageCallback;
        std::function<void()> boostCallback;
        
        void updatePosition();

    public:
        Rickshaw(float sx, float sy);
        ~Rickshaw() = default;

        void update(float dt) override;
        void draw(sf::RenderWindow& window) override;
        void takeDamage(int amount) override;
        int CalculateFare() override;
        Rickshaw& operator+(std::string upgrade);
        
        void setDamageCallback(std::function<void(int)> callback);
        void setBoostCallback(std::function<void()> callback);

        // Upgrade support
        void setTurbo(bool on) { hasTurbo = on; }
        bool getHasTurbo() const { return hasTurbo; }
        void applyPaint(int index);
        void resetVisuals();
        
        float getX() const { return x; }
        float getY() const { return y; }
        void setPosition(float nx, float ny) { x = nx; y = ny; }
        
        sf::FloatRect getBounds() const { return body.getGlobalBounds(); }
    };

    
    //  Template Usage for: Generic Vehicle Repair System
    
    class Mechanic {
    public:
        template <typename VehicleType>
        static void repair(VehicleType& v) {
            v.template updateStat<int>(v.health, 100);    // Health refill
            v.refillFuel();                               // Fuel refill to max capacity
        }
    };
}

#endif