#ifndef INTERFACES_H
#define INTERFACES_H

#include <string>
#include <vector>

namespace KUET {
    class IHittable {
    public:
        virtual void takeDamage(int amount) = 0;
        virtual ~IHittable() {}
    };

    enum class RequirementType {
        TIME_ONLY,
        NO_DAMAGE,
        FUEL_LIMIT,
        NO_BOOST,
        MAX_SPEED,
        COLLECT_ITEMS
    };

    struct MissionRequirement {
        RequirementType type;
        float value;
        std::string description;
        bool achieved;
        
        MissionRequirement(RequirementType t, float v, std::string desc)
            : type(t), value(v), description(desc), achieved(false) {}
    };

    class IMission {
    protected:
        int missionStatus = 0;      // 0=Inactive, 1=Active, 2=Completed, 3=Failed
        std::string dialogue;
        float timeLimit = 60.0f;
        float initialTimeLimit = 60.0f;
        bool timerStarted = false;
        std::vector<MissionRequirement> requirements;
        
    public:
        virtual void Start(void* playerPtr) = 0;
        
        virtual void Update(float dt, void* playerPtr) {
            if (missionStatus == 1 && timerStarted) {
                timeLimit -= dt;
                if (timeLimit <= 0.0f) {
                    missionStatus = 3;
                    dialogue = "Time's up! Mission Failed!";
                }
            }
        }
        
        void startTimer() { 
            timerStarted = true; 
        }
        
        bool isTimerStarted() const { return timerStarted; }
        
        void addRequirement(RequirementType type, float value, std::string desc) {
            requirements.push_back(MissionRequirement(type, value, desc));
        }
        
        virtual void checkRequirements(void* playerPtr) {}
        
        std::string getDialogue() const { return dialogue; }
        int getStatus() const { return missionStatus; }
        float getTimeLeft() const { return timeLimit; }
        float getTimeLimit() const { return initialTimeLimit; }
        
        virtual ~IMission() {}
    };
}

#endif