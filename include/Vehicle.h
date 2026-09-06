#ifndef VEHICLE_H
#define VEHICLE_H

#include "GameObject.h"
#include "Interfaces.h"
#include <SFML/Graphics.hpp>

namespace KUET {
    class Vehicle : public GameObject, public IHittable {
    protected:
        float speed, fuel;
        int health;

        // Upgrade-controllable modifiers
        float maxFuel;
        float speedMult;    // 1.0 base, upgrades raise it
        float armorMult;    // 1.0 = full damage, upgrades lower it

    public:
        Vehicle(float sx, float sy)
            : GameObject(sx, sy), speed(200.0f), fuel(100.0f), health(100),
              maxFuel(100.0f), speedMult(1.0f), armorMult(1.0f) {}
        
        
        // Template for generic programming
        
        template <typename T>
        void updateStat(T& stat, T amount) {
            stat += amount;
            if (stat > 100) stat = 100;
            if (stat < 0) stat = 0;
        }

        virtual int CalculateFare() = 0;
        
        float getFuel() { return fuel; }
        int getHealth() { return health; }
        
        // Upgrade accessors
        void setMaxFuel(float cap) { maxFuel = cap; if (fuel > cap) fuel = cap; }
        float getMaxFuel() { return maxFuel; }
        void setSpeedMult(float m) { speedMult = m; }
        float getSpeedMult() { return speedMult; }
        void setArmorMult(float m) { armorMult = m; }
        void refillFuel() { fuel = maxFuel; }
        
        void takeDamage(int amount) override { 
            int real = (int)(amount * armorMult);
            if (real < 1) real = 1;
            updateStat<int>(health, -real); 
        }
    };
}

#endif