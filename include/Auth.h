#ifndef AUTH_H
#define AUTH_H

#include <SFML/Graphics.hpp>
#include <string>

namespace KUET {

    class UserProfile {
    private:
        std::string username;
        std::string password;
        int totalEarnings;
        int unlockedMission;
        int highestScore;

        // Rickshaw upgrades (persisted)
        int speedUpgrade;     // 0..3 raises top speed
        int fuelUpgrade;      // 0..3 raises fuel capacity
        int armorUpgrade;     // 0..3 reduces damage taken
        int turboUpgrade;     // 0/1 permanent turbo unlock
        int paintIndex;       // 0..4 paint scheme

    public:
        UserProfile();
        UserProfile(std::string uname, std::string pass, int earnings = 0, int mission = 1, int score = 0,
                    int speed = 0, int fuel = 0, int armor = 0, int turbo = 0, int paint = 0);

        std::string getUsername() const;
        std::string getPassword() const;
        int getTotalEarnings() const;
        int getUnlockedMission() const;
        int getHighestScore() const;

        int getSpeedUpgrade() const;
        int getFuelUpgrade() const;
        int getArmorUpgrade() const;
        int getTurboUpgrade() const;
        int getPaintIndex() const;

        void addEarnings(int amount);
        void spendEarnings(int amount);
        void updateUnlockedMission(int missionNumber);
        void updateHighestScore(int score);
        void setSpeedUpgrade(int v);
        void setFuelUpgrade(int v);
        void setArmorUpgrade(int v);
        void setTurboUpgrade(int v);
        void setPaintIndex(int v);
    };

    class AuthManager {
    private:
        static UserProfile currentUser;
        static bool isLoggedIn;

    public:
        static bool checkIsLoggedIn();
        static UserProfile& getCurrentUser();

        static bool loginUser(const std::string& username, const std::string& password);
        static bool registerUser(const std::string& username, const std::string& password);
        static void saveProgress(int extraEarnings, int highestMissionCompleted, int currentScore);
        static void saveUpgrades();
        static void logout();
    };

    // UI Screen States
    class ScreenState {
    public:
        virtual ~ScreenState() = default;
        virtual ScreenState* handleInput(const sf::Event& event) = 0;
        virtual void render(sf::RenderWindow& window, const sf::Font& font) = 0;
    };

    class LoginScreen : public ScreenState {
    private:
        std::string uname = "", pass = "";
        bool typingUname = true;
        float animTimer = 0.0f;
        float cursorTimer = 0.0f;
        std::string statusMsg;
        float statusTimer = 0.0f;
    public:
        ScreenState* handleInput(const sf::Event& event) override;
        void render(sf::RenderWindow& window, const sf::Font& font) override;
    };

    class DashboardScreen : public ScreenState {
    private:
        float animTimer = 0.0f;
    public:
        ScreenState* handleInput(const sf::Event& event) override;
        void render(sf::RenderWindow& window, const sf::Font& font) override;
    };

    class PlayingScreen : public ScreenState {
    private:
        int selectedMission;
        bool isCustom = false;
        float cPickupX = 0, cPickupY = 0, cDestX = 0, cDestY = 0;
        int cFare = 0;
        float cTime = 0;
    public:
        PlayingScreen(int missionIdx = 0) : selectedMission(missionIdx) {}
        // Custom mission: selectedMission is sentinel -1 (no story mission index)
        PlayingScreen(float pX, float pY, float dX, float dY, int fare, float time)
            : selectedMission(-1), isCustom(true),
              cPickupX(pX), cPickupY(pY), cDestX(dX), cDestY(dY), cFare(fare), cTime(time) {}
        int getSelectedMission() const { return selectedMission; }
        bool isCustomMission() const { return isCustom; }
        float getCustomPickupX() const { return cPickupX; }
        float getCustomPickupY() const { return cPickupY; }
        float getCustomDestX() const { return cDestX; }
        float getCustomDestY() const { return cDestY; }
        int getCustomFare() const { return cFare; }
        float getCustomTime() const { return cTime; }
        ScreenState* handleInput(const sf::Event& event) override;
        void render(sf::RenderWindow& window, const sf::Font& font) override;
    };

    // Mission builder: pick a pickup point and a destination on the campus map
    class CustomMissionScreen : public ScreenState {
    private:
        float cursorX = 1000, cursorY = 750;
        float pickupX = 0, pickupY = 0;
        float destX = 0, destY = 0;
        bool pickupSet = false;
        bool destSet = false;
        int fare = 100;
        float timeLimit = 60.0f;
        float animTimer = 0.0f;
        std::string statusMsg;
        float statusTimer = 0.0f;
    public:
        CustomMissionScreen();
        ScreenState* handleInput(const sf::Event& event) override;
        void render(sf::RenderWindow& window, const sf::Font& font) override;
    };

    class UpgradeScreen : public ScreenState {
    private:
        float animTimer = 0.0f;
        std::string statusMsg;
        float statusTimer = 0.0f;
        int selected = 0; // hovered option
    public:
        ScreenState* handleInput(const sf::Event& event) override;
        void render(sf::RenderWindow& window, const sf::Font& font) override;
    };
}

#endif
