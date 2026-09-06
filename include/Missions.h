#ifndef MISSIONS_H
#define MISSIONS_H

#include "Rickshaw.h"
#include "CustomExceptions.h"
#include "Interfaces.h"
#include <cmath>
#include <string>
using namespace std;
namespace KUET {

// Base Passenger Mission Class
class PassengerMission : public IMission {
protected:
    string passengerName;
    string destinationName;
    float targetX, targetY;
    int fareReward;
    bool passengerPicked = false;
    bool passengerDropped = false;
    bool missionCompleted = false;
    Rickshaw* playerRef = nullptr;
    float startFuel = 0;
    int damageTaken = 0;
    bool usedBoost = false;
    float completeMessageTimer = 0;
    
public:
    PassengerMission(string passName, string destName, float destX, float destY, int fare)
        : passengerName(passName), destinationName(destName), targetX(destX), targetY(destY), fareReward(fare) {}
    
    //  Getters added for Dynamic Green Signal
    float getTargetX() const { return targetX; }
    float getTargetY() const { return targetY; }

    void Start(void* p) override {
        playerRef = (Rickshaw*)p;
        missionStatus = 1;
        timeLimit = initialTimeLimit;
        timerStarted = false;
        passengerPicked = false;
        passengerDropped = false;
        missionCompleted = false;
        completeMessageTimer = 0;
        startFuel = playerRef->getFuel();
        damageTaken = 0;
        usedBoost = false;
        dialogue = "Find and pick up: " + passengerName + "!\nPress SPACE near them to start mission!";
    }
    
    void setPassengerPicked() { 
        passengerPicked = true; 
        startTimer();
        dialogue = passengerName + " is in rickshaw!\nTake them to " + destinationName + "!\n" + getRequirementText();
    }
    
    void recordDamage(int dmg) { damageTaken += dmg; }
    void recordBoost() { usedBoost = true; }
    
    string getRequirementText() {
        string text = "";
        for (auto& req : requirements) {
            text += req.description + " ";
        }
        return text;
    }
    
    bool isMissionCompleted() const { return missionCompleted; }
    float getCompleteMessageTimer() const { return completeMessageTimer; }
    
    void checkRequirements() {
        for (auto& req : requirements) {
            switch(req.type) {
                case RequirementType::TIME_ONLY:
                    req.achieved = true;
                    break;
                case RequirementType::NO_DAMAGE:
                    if (damageTaken > 0) {
                        req.achieved = false;
                        dialogue = "Failed: You took damage! Mission Failed!";
                        missionStatus = 3;
                    } else {
                        req.achieved = true;
                    }
                    break;
                case RequirementType::FUEL_LIMIT:
                    if (playerRef->getFuel() < req.value) {
                        req.achieved = false;
                        dialogue = "Failed: Fuel below " + std::to_string((int)req.value) + "%! Mission Failed!";
                        missionStatus = 3;
                    } else {
                        req.achieved = true;
                    }
                    break;
                case RequirementType::NO_BOOST:
                    if (usedBoost) {
                        req.achieved = false;
                        dialogue = "Failed: You used boost! Mission Failed!";
                        missionStatus = 3;
                    } else {
                        req.achieved = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    void Update(float dt, void* p) override {
        IMission::Update(dt, p);
        
        playerRef = (Rickshaw*)p;
        
        if (missionCompleted) {
            completeMessageTimer += dt;
            if (completeMessageTimer >= 3.0f) {
                missionStatus = 2;
            }
            return;
        }
        
        if (!passengerPicked) {
            dialogue = "Find and pick up: " + passengerName + "!\nPress SPACE near them to start mission!";
        } 
        else if (!passengerDropped) {
            float dx = playerRef->getX() - targetX;
            float dy = playerRef->getY() - targetY;
            float distance = std::sqrt(dx*dx + dy*dy);
            
            std::string timeStr = timerStarted ? std::to_string((int)timeLimit) + "s left" : "Timer not started";
            dialogue = passengerName + " wants to go to " + destinationName + "!\n"
                      "Distance: " + std::to_string((int)distance) + "m | " + timeStr + "\n"
                      + getRequirementText();
            
            if (distance < 100.0f) {
                checkRequirements();
                if (missionStatus != 3) {
                    passengerDropped = true;
                    missionCompleted = true;
                    dialogue = "MISSION COMPLETE! +" + std::to_string(fareReward) + " TK!\nPress ENTER to continue...";
                }
            }
        }
    }
    
    bool isPassengerPicked() const { return passengerPicked; }
    bool isPassengerDropped() const { return passengerDropped; }
    int getFareReward() const { return fareReward; }
    std::string getPassengerName() const { return passengerName; }
    bool isMissionCompletedFlag() const { return missionCompleted; }
};


// MISSION 1: Main Gate to CSE Building

class Mission1 : public PassengerMission {
public:
    Mission1() : PassengerMission("Rahim (CSE Student)", "CSE Building", 992.0f, 775.0f, 50) {
        initialTimeLimit = 60.0f;
        addRequirement(RequirementType::TIME_ONLY, 60.0f, "[Time: 60s]");
    }
};


// MISSION 2: CSE Building to KUET Library

class Mission2 : public PassengerMission {
public:
    Mission2() : PassengerMission("Sultana (Library)", "KUET Library", 1197.0f, 227.0f, 75) {
        initialTimeLimit = 50.0f;
        addRequirement(RequirementType::TIME_ONLY, 50.0f, "[Time: 50s]");
        addRequirement(RequirementType::NO_DAMAGE, 0, "[No Damage]");
    }
};


// MISSION 3: KUET Library → Shaheed Minar

class Mission3 : public PassengerMission {
public:
    Mission3() : PassengerMission("Shahid (Student)", "Shaheed Minar", 1416.0f, 1210.0f, 60) {
        initialTimeLimit = 55.0f;
        addRequirement(RequirementType::TIME_ONLY, 55.0f, "[Time: 55s]");
        addRequirement(RequirementType::FUEL_LIMIT, 50.0f, "[Fuel > 50%]");
    }
};


// MISSION 4: Shaheed Minar → Amar Ekushey Hall

class Mission4 : public PassengerMission {
public:
    Mission4() : PassengerMission("Liton (Hall Student)", "Amar Ekushey Hall", 680.0f, 1275.0f, 80) {
        initialTimeLimit = 40.0f;
        addRequirement(RequirementType::TIME_ONLY, 40.0f, "[Time: 40s]");
        addRequirement(RequirementType::NO_BOOST, 0, "[No Boost]");
    }
};


// MISSION 5: Amar Ekushey Hall → KUET Main Gate

class Mission5 : public PassengerMission {
public:
    Mission5() : PassengerMission("Gate er Guard", "KUET Main Gate", 1805.0f, 1370.0f, 150) {
        initialTimeLimit = 70.0f;
        addRequirement(RequirementType::TIME_ONLY, 70.0f, "[Time: 70s]");
        addRequirement(RequirementType::NO_DAMAGE, 0, "[No Damage]");
        addRequirement(RequirementType::FUEL_LIMIT, 40.0f, "[Fuel > 40%]");
        addRequirement(RequirementType::NO_BOOST, 0, "[No Boost]");
    }
};


// CUSTOM MISSION: player-chosen pickup + destination (Dashboard -> C -> Mission Builder)

class CustomMission : public PassengerMission {
protected:
    float pickupX = 0.0f, pickupY = 0.0f;

public:
    CustomMission(float pX, float pY, float dX, float dY, int fare, float time,
                  const string& pName, const string& dName)
        : PassengerMission(pName, dName, dX, dY, fare), pickupX(pX), pickupY(pY) {
        initialTimeLimit = time;
        addRequirement(RequirementType::TIME_ONLY, time,
                       "[Time: " + std::to_string((int)time) + "s]");
    }

    float getPickupX() const { return pickupX; }
    float getPickupY() const { return pickupY; }
};

}

#endif