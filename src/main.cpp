
#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include "../include/Constants.h"
#include "../include/Campus.h"
#include "../include/CustomExceptions.h"
#include "../include/Rickshaw.h"
#include "../include/Passenger.h"
#include "../include/Missions.h"
#include "../include/Minimap.h"
#include "../include/Environment.h"
#include "../include/Auth.h"
#include "../include/SoundManager.h"
using namespace KUET;
using namespace std;

// Building structure (shared campus layout from Campus.cpp)
using Building = CampusBuilding;

// Obstacle structure 
struct Obstacle {
    std::string type;
    float x, y;
    int damage;
};

// Pond
struct Pond {
    float x, y, width, height;
    std::string label;
};

// Road network segment (matches the drawn roads; shared from Campus.cpp)
using RoadSegment = CampusRoad;

// Returns the closest point on a road's center-line to (px, py)
sf::Vector2f nearestRoadCenter(const RoadSegment& r, float px, float py) {
    if (r.isVertical) {
        float laneX = r.x + r.width * 0.5f;
        float cly = std::max(r.y, std::min(py, r.y + r.height));
        return sf::Vector2f(laneX, cly);
    } else {
        float laneY = r.y + r.height * 0.5f;
        float clx = std::max(r.x, std::min(px, r.x + r.width));
        return sf::Vector2f(clx, laneY);
    }
}

// Soft road-snap: gently pull a moving vehicle toward the nearest road center-line.
void applyRoadSnap(Rickshaw& rk, const std::vector<RoadSegment>& roads,
                   float snapRadius, float strength, bool moving) {
    if (!moving) return;
    float bestD2 = snapRadius * snapRadius;
    sf::Vector2f bestTarget;
    bool found = false;
    float px = rk.getX(), py = rk.getY();
    for (const auto& r : roads) {
        sf::Vector2f t = nearestRoadCenter(r, px, py);
        float dx = t.x - px, dy = t.y - py;
        float d2 = dx * dx + dy * dy;
        if (d2 < bestD2) {
            found = true;
            bestD2 = d2;
            bestTarget = t;
        }
    }
    if (found) {
        float nx = px + (bestTarget.x - px) * strength;
        float ny = py + (bestTarget.y - py) * strength;
        rk.setPosition(nx, ny);
    }
}

// tree draw
static void drawTree(sf::RenderWindow& w, float x, float y, sf::Color leafCol = sf::Color(34,139,34)) {
    sf::RectangleShape trunk(sf::Vector2f(12.f, 30.f));
    trunk.setFillColor(sf::Color(101,67,33));
    trunk.setPosition(sf::Vector2f(x, y));
    w.draw(trunk);
    sf::CircleShape leaves(22.f);
    leaves.setFillColor(leafCol);
    leaves.setPosition(sf::Vector2f(x - 16.f, y - 30.f));
    w.draw(leaves);
}

// draw playground
static void drawPlayField(sf::RenderWindow& w, float x, float y, float ww, float hh, const std::string& lbl, const sf::Font& font) {
    sf::RectangleShape field(sf::Vector2f(ww, hh));
    field.setFillColor(sf::Color(56, 160, 56, 200));
    field.setOutlineThickness(3);
    field.setOutlineColor(sf::Color(255, 255, 255, 100));
    field.setPosition(sf::Vector2f(x, y));
    w.draw(field);
    sf::Text t(font, lbl, 11);
    t.setFillColor(sf::Color(255,255,200));
    t.setOutlineColor(sf::Color::Black);
    t.setOutlineThickness(1);
    t.setPosition(sf::Vector2f(x + 4.f, y + 4.f));
    w.draw(t);
}

// draw garden zone 
static void drawGarden(sf::RenderWindow& w, float x, float y, float ww, float hh, const std::string& lbl, const sf::Font& font) {
    sf::RectangleShape g(sf::Vector2f(ww, hh));
    g.setFillColor(sf::Color(0, 100, 50, 180));
    g.setOutlineThickness(2);
    g.setOutlineColor(sf::Color(0,180,80,200));
    g.setPosition(sf::Vector2f(x, y));
    w.draw(g);
    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 4; ++j) {
            sf::CircleShape dot(8.f);
            dot.setFillColor(sf::Color(30, 130, 30, 200));
            dot.setPosition(sf::Vector2f(x + 10.f + i * (ww/6.f), y + 8.f + j * (hh/4.f)));
            w.draw(dot);
        }
    sf::Text t(font, lbl, 11);
    t.setFillColor(sf::Color(200,255,150));
    t.setOutlineColor(sf::Color::Black);
    t.setOutlineThickness(1);
    t.setPosition(sf::Vector2f(x + 4.f, y + 4.f));
    w.draw(t);
}

// lamp post with warm light
static void drawLamp(sf::RenderWindow& w, float x, float y, float anim = 0.f, sf::Color lightCol = sf::Color(255, 220, 120)) {
    sf::RectangleShape pole(sf::Vector2f(5, 42));
    pole.setFillColor(sf::Color(45, 45, 50));
    pole.setPosition(sf::Vector2f(x, y));
    w.draw(pole);
    sf::RectangleShape arm(sf::Vector2f(18, 4));
    arm.setFillColor(sf::Color(45, 45, 50));
    arm.setPosition(sf::Vector2f(x - 5, y - 8));
    w.draw(arm);
    sf::CircleShape bulb(5);
    float flicker = 0.8f + 0.2f * std::sin(anim * 6.0f + x);
    bulb.setFillColor(sf::Color((std::uint8_t)(lightCol.r * flicker),
                                (std::uint8_t)(lightCol.g * flicker),
                                (std::uint8_t)(lightCol.b * flicker)));
    bulb.setPosition(sf::Vector2f(x + 6, y - 15));
    w.draw(bulb);
    // faint halo
    sf::CircleShape halo(18);
    halo.setFillColor(sf::Color(255, 220, 120, 25));
    halo.setPosition(sf::Vector2f(x - 11, y - 28));
    w.draw(halo);
}

// park bench
static void drawBench(sf::RenderWindow& w, float x, float y) {
    sf::RectangleShape seat(sf::Vector2f(34, 6));
    seat.setFillColor(sf::Color(139, 95, 40));
    seat.setPosition(sf::Vector2f(x, y));
    w.draw(seat);
    sf::RectangleShape back(sf::Vector2f(34, 8));
    back.setFillColor(sf::Color(120, 80, 35));
    back.setPosition(sf::Vector2f(x, y - 10));
    w.draw(back);
    sf::RectangleShape leg1(sf::Vector2f(4, 8));
    leg1.setFillColor(sf::Color(90, 60, 25));
    leg1.setPosition(sf::Vector2f(x + 2, y + 6));
    w.draw(leg1);
    sf::RectangleShape leg2(sf::Vector2f(4, 8));
    leg2.setFillColor(sf::Color(90, 60, 25));
    leg2.setPosition(sf::Vector2f(x + 28, y + 6));
    w.draw(leg2);
}

// flower bed
static void drawFlowerBed(sf::RenderWindow& w, float x, float y, float ww, float hh) {
    sf::RectangleShape bed(sf::Vector2f(ww, hh));
    bed.setFillColor(sf::Color(101, 67, 33));
    bed.setPosition(sf::Vector2f(x, y));
    w.draw(bed);
    for (int i = 0; i < (int)(ww / 14); ++i)
        for (int j = 0; j < (int)(hh / 14); ++j) {
            static const sf::Color cols[4] = {
                sf::Color(255, 80, 120), sf::Color(120, 200, 80),
                sf::Color(255, 200, 60), sf::Color(180, 120, 255)
            };
            sf::CircleShape fl(3.f);
            fl.setFillColor(cols[(i + j * 3) % 4]);
            fl.setPosition(sf::Vector2f(x + 7.f + i * 14.f, y + 7.f + j * 14.f));
            w.draw(fl);
        }
}

// zebra crossing on a road (rx,ry = top-left of road, horizontal)
static void drawZebraCrossing(sf::RenderWindow& w, float x, float y, float roadH) {
    for (int i = 0; i < 6; ++i) {
        sf::RectangleShape stripe(sf::Vector2f(14, 10));
        stripe.setFillColor(sf::Color(255, 255, 255, 200));
        stripe.setPosition(sf::Vector2f(x + i * 26, y + roadH * 0.3f));
        w.draw(stripe);
    }
}

// sign post
static void drawSignPost(sf::RenderWindow& w, float x, float y, const std::string& txt, const sf::Font& font) {
    sf::RectangleShape pole(sf::Vector2f(4, 30));
    pole.setFillColor(sf::Color(60, 60, 65));
    pole.setPosition(sf::Vector2f(x, y));
    w.draw(pole);
    sf::RectangleShape board(sf::Vector2f(46, 14));
    board.setFillColor(sf::Color(20, 70, 150));
    board.setPosition(sf::Vector2f(x - 8, y - 12));
    w.draw(board);
    sf::Text t(font, txt, 9);
    t.setFillColor(sf::Color::White);
    t.setPosition(sf::Vector2f(x - 4, y - 11));
    w.draw(t);
}

int main() {
    std::cout << "\n========== KUET RICKSHAW RUSH STARTING ==========\n";
    std::cout << "Initializing game...\n";
    
    const int   WINDOW_WIDTH  = sf::VideoMode::getDesktopMode().size.x - 100;
const int   WINDOW_HEIGHT = sf::VideoMode::getDesktopMode().size.y - 100;
    const float WORLD_WIDTH   = 2000.f;
    const float WORLD_HEIGHT  = 1500.f;

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)),
        "KUET Rickshaw Rush - Full Campus Map");
    window.setFramerateLimit(60);
    std::cout << "[OK] Window created: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << "\n";
    SoundManager::init();
    std::cout << "[OK] Sound system initialized\n";

    if (SoundManager::startMusic("assets/bg music.mp3"))
        std::cout << "[OK] Background music loaded: assets/bg music.mp3\n";
    else
        std::cout << "[WARN] Background music could not be loaded\n";

    sf::View camera(sf::FloatRect(sf::Vector2f(0, 0),
                                  sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)));

    // Font
    sf::Font font;
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/seguiemj.ttf"
    };
    bool fontLoaded = false;
    for (const auto& p : fontPaths) {
        if (font.openFromFile(p)) { 
            fontLoaded = true; 
            std::cout << "[OK] Font loaded: " << p << "\n";
            break; 
        }
    }
    if (!fontLoaded) { 
        std::cerr << "[ERROR] No font found!\n"; 
        return -1; 
    }
    Environment env;
    env.setTime(0);
    ScreenState* currentState = new LoginScreen();
    sf::Text uiText(font, "", 28);
    uiText.setFillColor(sf::Color::White);
    uiText.setOutlineThickness(1);
    uiText.setOutlineColor(sf::Color::Black);

    // ── Player start Kuet main gate
    Rickshaw* player = new Rickshaw(1805.f, 1370.f);
    std::cout << "[OK] Player created at (1805, 1370)\n";

    // Apply saved garage upgrades (paint, speed, fuel tank, armor, permanent turbo)
    auto applyUpgradesToPlayer = [&]() {
        UserProfile& up = AuthManager::getCurrentUser();
        player->applyPaint(up.getPaintIndex());
        player->setSpeedMult(1.0f + up.getSpeedUpgrade() * 0.15f);
        player->setMaxFuel(100.0f + up.getFuelUpgrade() * 50.0f);
        player->setArmorMult(1.0f - up.getArmorUpgrade() * 0.20f);
        player->setTurbo(up.getTurboUpgrade() > 0);
        player->refillFuel();  // fill to the (possibly larger) upgraded tank
    };

//Buildings (layout shared with the Custom Mission builder via Campus.cpp)
    vector<Building> buildings = buildCampusBuildings();
    cout << "[OK] " << buildings.size() << " buildings loaded\n";

    // Road network (shared with the Custom Mission builder; must match the draw loop)
    vector<RoadSegment> roads = buildCampusRoads();
    cout << "[OK] " << roads.size() << " road segments loaded\n";

    // Minimap
    vector<MinimapBuilding> minimapBuildings;
    for (const auto& b : buildings)
        minimapBuildings.push_back({b.name, b.x, b.y, b.width, b.height, b.color});

    Minimap minimap(WORLD_WIDTH, WORLD_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, font);
    minimap.updateBuildings(minimapBuildings);
    cout << "[OK] Minimap initialized\n";

  
    // PONDS implement
  
    vector<Pond> ponds = {
        { 75,  520, 155, 230, "Lake"},
        {1305,  490, 130, 110, "Pond"},
        {1510, 1260, 160, 145, "Pond"},
        {1055, 1195, 110, 100, "Pond"},
        { 620,  920, 100,  80, "Pond"},
    };
    cout << "[OK] " << ponds.size() << " ponds loaded\n";

  
    // OBSTACLES
  
    vector<Obstacle> obstacles = {
        {"pothole",      1760, 1430, 15},
        {"speed_breaker",1650, 1430,  5},
        {"pothole",       700, 1430, 15},
        {"speed_breaker", 880,  870,  5},
        {"pothole",       490, 1430, 15},
        {"speed_breaker",1050,  820,  5},
        {"pothole",       390,  700, 15},
        {"speed_breaker", 390,  400,  5},
        {"pothole",      1600,  870, 15},
    };
    cout << "[OK] " << obstacles.size() << " obstacles loaded\n";


    // PASSENGERS (pickup locations) 
  
    vector<Passenger*> passengers;
    
    passengers.push_back(new Passenger(1805, 1370, "Rahim (CSE)", "CSE Building", 50));
   
    passengers.push_back(new Passenger(948, 840, "Sultana (Lib)", "KUET Library", 75));
    
    passengers.push_back(new Passenger(1120, 175, "Shahid (Student)", "Shaheed Minar", 60));
    
    passengers.push_back(new Passenger(1390, 1170, "Liton (Hall)", "Amar Ekushey Hall", 80));

    passengers.push_back(new Passenger(680, 1275, "Gate er Guard", "KUET Main Gate", 150));

    for (int i = 0; i < (int)passengers.size(); i++)
        passengers[i]->setMissionId(i);
    cout << "[OK] " << passengers.size() << " passengers loaded\n";

  
    // MISSIONS
  
    vector<PassengerMission*> missions;
    missions.push_back(new Mission1());
    missions.push_back(new Mission2());
    missions.push_back(new Mission3());
    missions.push_back(new Mission4());
    missions.push_back(new Mission5());
    cout << "[OK] 5 missions loaded\n";

    int  currentMissionIdx    = 0;
    bool waitingForNextMission = false;
    bool isCustomMission      = false;
    PassengerMission* customMission   = nullptr;
    Passenger*        customPassenger = nullptr;
    applyUpgradesToPlayer();
    missions[currentMissionIdx]->Start(player);
    cout << "[OK] Mission " << currentMissionIdx+1 << " started: " 
              << missions[currentMissionIdx]->getPassengerName() << "\n";

    player->setDamageCallback([&](int dmg) {
        PassengerMission* ac = isCustomMission ? customMission
                            : (currentMissionIdx >= 0 && currentMissionIdx < (int)missions.size())
                                   ? missions[currentMissionIdx] : nullptr;
        if (ac && !waitingForNextMission) ac->recordDamage(dmg);
    });
    player->setBoostCallback([&]() {
        PassengerMission* ac = isCustomMission ? customMission
                            : (currentMissionIdx >= 0 && currentMissionIdx < (int)missions.size())
                                   ? missions[currentMissionIdx] : nullptr;
        if (ac && !waitingForNextMission) ac->recordBoost();
    });

    

  
    // TREE POSITIONS
  
    vector<sf::Vector2f> treePosns;
    for (float x = 430; x < 1760; x += 120) treePosns.push_back({x, 80});
    for (float y = 120; y < 1370; y += 120) treePosns.push_back({1870, y});
    for (float x = 430; x < 1800; x += 130) treePosns.push_back({x, 1370});
    for (float y = 80; y < 1440; y += 130) treePosns.push_back({415, y});
    for (float x = 630; x < 1400; x += 160) {
        treePosns.push_back({x, 845});
        treePosns.push_back({x, 795});
    }
    for (float x = 1460; x < 1740; x += 80) {
        treePosns.push_back({x,  70});
        treePosns.push_back({x, 230});
    }
    cout << "[OK] " << treePosns.size() << " trees positioned\n";

    // DECORATIVE ELEMENTS (lamps / benches / flower beds / signs)
    vector<sf::Vector2f> lampPosns = {
        { 470,  1400}, { 620, 1400}, { 920, 1400}, {1160, 1400}, {1580, 1400}, {1740, 1400},
        { 420,   90},  { 680,  90},  { 980,  90},  {1280,  90},  {1500,  90},  {1760,  90},
        { 400,  480},  { 400,  700}, { 620,  850}, { 780,  850}, {1040,  380}, {1200,  900},
        { 600, 1260},  { 980, 1260}, {1380,  600}, {1380, 1000}, {1640, 1300}, {1900,  300},
    };
    vector<sf::Vector2f> benchPosns = {
        { 470, 1360}, { 700, 1230}, {1390, 1160}, {430, 520}, {1010, 800}, {1460, 350},
    };
    vector<sf::Vector2f> flowerPosns = {
        { 424,  300}, { 424,  520}, {1250, 1080}, {1470, 1100}, { 500, 1210}, { 950, 1015},
    };
    vector<std::pair<sf::Vector2f, std::string>> signPosns = {
        {{ 428, 1180}, "HALLS ->"},
        {{ 620,  800}, "CSE ->"},
        {{1030,  575}, "LIBRARY ->"},
        {{1380,  350}, "LIBRARY ->"},
    };
    cout << "[OK] " << lampPosns.size() << " lamps, " << benchPosns.size()
         << " benches, " << flowerPosns.size() << " flower beds positioned\n";

  
    // GAME LOOP SETUP
  
    sf::Clock clock;
    bool  isGameOver    = false;
    int   totalScore    = 0;
    float lastDamageTime = 0.f;
    float worldAnimTimer = 0.f;
    float fpsTimer = 0.f;
    int frameCount = 0;
    bool  paused    = false;     // P/Esc pause menu
    bool  helpOpen  = false;     // H help / controls overlay
    bool  backToDashboard = false;

    cout << "\n========== GAME STARTED! ==========\n";
    cout << "Controls: WASD=Move | Shift=Boost | SPACE=Pickup | ENTER=Next | R=Repair | P=Pause | H=Help\n\n";

    
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        worldAnimTimer += dt;
        lastDamageTime += dt;
        
        // Active mission/passenger: a story mission, or a player-built custom mission
        PassengerMission* activeMission = nullptr;
        Passenger* activePassenger = nullptr;
        if (isCustomMission && customMission && customPassenger) {
            activeMission = customMission;
            activePassenger = customPassenger;
        } else if (currentMissionIdx >= 0 && currentMissionIdx < (int)missions.size()) {
            activeMission = missions[currentMissionIdx];
            if (currentMissionIdx < (int)passengers.size())
                activePassenger = passengers[currentMissionIdx];
        }
        
        
        fpsTimer += dt;
        frameCount++;
        if (fpsTimer >= 1.0f) {
            // cout << "FPS: " << frameCount << "\n";  testing
            fpsTimer = 0.f;
            frameCount = 0;
        }

       //Events
while (const optional<sf::Event> ev = window.pollEvent()) {
        if (ev->is<sf::Event::Closed>()) {
            cout << "[INFO] Window closed by user\n";
            window.close();
        }

        
        ScreenState* nextState = currentState->handleInput(*ev);
       // State change handle kora (main loop er vitore)
        if (nextState != currentState) {
            // Check korchi jodi notun state ta PlayingScreen hoy
            if (auto* ps = dynamic_cast<PlayingScreen*>(nextState)) {
                // Game er state reset kore dilam jate fresh bhabe shuru hoy
                isGameOver = false;
                waitingForNextMission = false;
                applyUpgradesToPlayer();

                if (ps->isCustomMission()) {
                    // Custom mission zeemon pickup + destination dashboard theke bachano hoyeche
                    currentMissionIdx = -1;
                    isCustomMission = true;
                    delete customPassenger;
                    delete customMission;
                    customMission = new CustomMission(
                        ps->getCustomPickupX(), ps->getCustomPickupY(),
                        ps->getCustomDestX(), ps->getCustomDestY(),
                        ps->getCustomFare(), ps->getCustomTime(),
                        "Custom Rider", "Custom Drop");
                    customPassenger = new Passenger(
                        ps->getCustomPickupX(), ps->getCustomPickupY(),
                        "Custom Rider", "Custom Drop", ps->getCustomFare());
                    customPassenger->setMissionId(-1);
                    customMission->Start(player);
                    auto* cmPtr = static_cast<CustomMission*>(customMission);
                    cout << "[CUSTOM] Custom mission started: pickup ("
                              << (int)cmPtr->getPickupX() << ", "
                              << (int)cmPtr->getPickupY() << ") -> drop ("
                              << (int)customMission->getTargetX() << ", "
                              << (int)customMission->getTargetY() << ") | fare "
                              << customMission->getFareReward() << " TK | "
                              << (int)customMission->getTimeLimit() << "s\n";
                } else {
                    // User dashboard theke je mission select koreche sheta main game e set kore dilam
                    currentMissionIdx = ps->getSelectedMission();
                    isCustomMission = false;
                    missions[currentMissionIdx]->Start(player); 
                    cout << "[MISSION] Dashboard theke mission " << currentMissionIdx+1 << " start holo!\n";
                }
            }
            delete currentState;
            currentState = nextState;
        }
    
    if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
        // Determine if we are inside an active mission (PlayingScreen)
        bool isPlayingNow = (dynamic_cast<PlayingScreen*>(currentState) != nullptr);

        // Pause toggle (P) during active gameplay
        if (isPlayingNow && !isGameOver && !waitingForNextMission && kp->code == sf::Keyboard::Key::P) {
            paused = !paused;
            SoundManager::play(SFX::Click);
            if (paused) helpOpen = false;
            cout << (paused ? "[PAUSE] Game paused (P/Esc to resume)\n" : "[PAUSE] Game resumed\n");
        }
        // Esc toggles the pause menu too (active play only)
        if (isPlayingNow && !isGameOver && !waitingForNextMission && kp->code == sf::Keyboard::Key::Escape) {
            paused = !paused;
            SoundManager::play(SFX::Click);
            if (paused) helpOpen = false;
        }
        // H = Help / controls overlay (works both paused and running)
        if (isPlayingNow && kp->code == sf::Keyboard::Key::H) {
            helpOpen = !helpOpen;
            SoundManager::play(SFX::Click);
            cout << (helpOpen ? "[HELP] Controls shown (H to close)\n" : "[HELP] Controls hidden\n");
        }
        // B = Back to dashboard (only while paused, so it can't be pressed by accident)
        if (isPlayingNow && paused && kp->code == sf::Keyboard::Key::B) {
            SoundManager::play(SFX::Click);
            backToDashboard = true;
            cout << "[MENU] Returning to Dashboard (B pressed)...\n";
        }
        // Q = Quit game (only while paused)
        if (isPlayingNow && paused && kp->code == sf::Keyboard::Key::Q) {
            window.close();
        }

        // F12 = take a screenshot (saved as PNG in the game folder)
        if (kp->code == sf::Keyboard::Key::F12) {
            static int shotCount = 0;
            shotCount++;
            sf::Vector2u wSize = window.getSize();
            sf::Image blank(wSize, sf::Color::Black);
            sf::Texture snap;
            (void)snap.loadFromImage(blank);   // establish dimensions
            snap.update(window);               // copy current back buffer
            std::string nn = shotCount < 10 ? "0" + std::to_string(shotCount) : std::to_string(shotCount);
            std::string fname = "kuet_rickshaw_shot_" + nn + ".png";
            if (snap.copyToImage().saveToFile(fname))
                std::cout << "[SHOT] Saved " << fname << "\n";
            else
                std::cout << "[SHOT] FAILED to save " << fname << "\n";
        }

        // Turbo (Mission 4+ or any custom mission)
        if (kp->code == sf::Keyboard::Key::T && (currentMissionIdx >= 3 || isCustomMission)) {
            *player = *player + "Turbo";
            SoundManager::play(SFX::Turbo);
            cout << "[ACTION] Turbo activated!\n";
        }
        
        // Enter (Next mission after completion)
        if (kp->code == sf::Keyboard::Key::Enter && waitingForNextMission) {
            waitingForNextMission = false;
            if (isCustomMission) {
                // Custom missions are one-shot rides: head back to the Dashboard
                SoundManager::play(SFX::Click);
                backToDashboard = true;
                cout << "[CUSTOM] Custom ride finished. Returning to Dashboard...\n";
            } else {
                currentMissionIdx++;
                if (currentMissionIdx < (int)missions.size()) {
                    missions[currentMissionIdx]->Start(player);
                    SoundManager::play(SFX::Click);
                    cout << "[MISSION] Starting mission " << currentMissionIdx+1 
                              << ": " << missions[currentMissionIdx]->getPassengerName() << "\n";
                } else {
                    isGameOver = true;
                    SoundManager::play(SFX::Complete);
                    cout << "[GAME] All missions completed! Final Score: " << totalScore << "\n";
                }
            }
        }
        
        // U key to change time 
        if (kp->code == sf::Keyboard::Key::U) {
            static int timeMode = 0;
            timeMode = (timeMode + 1) % 3;
            env.setTime(timeMode);
            SoundManager::play(SFX::Click);
            cout << "[TIME] Changed to mode: " << timeMode 
                      << " (0=Morning, 1=Afternoon, 2=Night)\n";
        }
    }
}

        // Back to Dashboard from the pause menu
        if (backToDashboard) {
            if (isCustomMission) {
                delete customPassenger;
                delete customMission;
                customPassenger = nullptr;
                customMission = nullptr;
            }
            for (auto& pc : passengers) pc->reset();
            paused = false;
            helpOpen = false;
            backToDashboard = false;
            isGameOver = false;
            waitingForNextMission = false;
            currentMissionIdx = 0;
            isCustomMission = false;
            totalScore = 0;
            player->setPosition(1805.f, 1370.f);
            Mechanic::repair(*player);
            delete currentState;
            currentState = new DashboardScreen();
            cout << "[MENU] Back at Dashboard. Pick a mission (1-5), press C for a custom mission, or open the Garage (U).\n";
        }

        // Logic
        if (!paused && !isGameOver && !waitingForNextMission) {

            // Engine / ambient sound based on movement + boost
            {
                bool moving = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
                bool boosting = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift);
                float speedFrac = moving ? 1.0f : 0.0f;
                SoundManager::setEngine(speedFrac, boosting && moving);
                // wind layer audible only at speed
                if (moving) SoundManager::startWind();
                else SoundManager::stopWind();
            }
            
            // 1. Exception check (try-catch )
            bool outOfFuel = GameExceptionHandler::checkFuelError(player->getFuel());
            bool timeOut = GameExceptionHandler::checkTimeError(activeMission->getStatus());

            if (outOfFuel) {
                // outoffuel-> Repair 
                uiText.setString("Chacha, tel shesh!\n[Press R to Repair]");
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
                    Mechanic::repair(*player);
                    cout << "[REPAIR] Rickshaw repaired! Fuel: 100% | Health: 100%\n";
                }
            } 
            else if (timeOut) {
                //timeup->gameover
                uiText.setString("Time Up! Mission Failed!\nGAME OVER");
                isGameOver = true;
            } 
else {
                bool rfMoving = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)
                             || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
                player->update(dt);
                env.update(dt);

                // Soft road-follow: gently keep the rickshaw on the road network
                applyRoadSnap(*player, roads, 60.f, 0.10f, rfMoving);

                sf::FloatRect pb = player->getBounds();

                // Building collision (gates, libraries, and open plazas are passable)
                for (const auto& b : buildings) {
                    if (b.name.find("Gate") != std::string::npos
                        || b.name.find("Library") != std::string::npos
                        || b.name.find("Minar") != std::string::npos
                        || b.name.find("Chattar") != std::string::npos
                        || b.name.find("Square") != std::string::npos)
                        continue;
                    sf::FloatRect bb(sf::Vector2f(b.x, b.y), sf::Vector2f(b.width, b.height));
                    if (pb.findIntersection(bb)) {
                        float overlapLeft   = (pb.position.x + pb.size.x) - bb.position.x;
                        float overlapRight  = (bb.position.x + bb.size.x) - pb.position.x;
                        float overlapTop    = (pb.position.y + pb.size.y) - bb.position.y;
                        float overlapBottom = (bb.position.y + bb.size.y) - pb.position.y;

                        float minOverlapX = std::min(overlapLeft, overlapRight);
                        float minOverlapY = std::min(overlapTop, overlapBottom);

                        if (minOverlapX < minOverlapY) {
                            if (overlapLeft < overlapRight)
                                player->setPosition(player->getX() - overlapLeft, player->getY());
                            else
                                player->setPosition(player->getX() + overlapRight, player->getY());
                        } else {
                            if (overlapTop < overlapBottom)
                                player->setPosition(player->getX(), player->getY() - overlapTop);
                            else
                                player->setPosition(player->getX(), player->getY() + overlapBottom);
                        }
                    }
                }

                // Obstacle collision
                for (const auto& obs : obstacles) {
                    sf::FloatRect ob(sf::Vector2f(obs.x - 15, obs.y - 10), sf::Vector2f(30, 20));
                    if (pb.findIntersection(ob) && lastDamageTime > 1.0f) {
                        player->takeDamage(obs.damage);
                        lastDamageTime = 0.f;
                        SoundManager::play(SFX::Damage);
                        cout << "[DAMAGE] Hit " << obs.type << "! Health: " 
                                  << player->getHealth() << "% | Damage: " << obs.damage << "\n";
                    }
                }

                // Passenger pickup
                PassengerMission* cm  = activeMission;
                Passenger* cp  = activePassenger;
                if (!cm->isPassengerPicked() && !cp->isPicked()) {
                    cp->update(player);
                    if (cp->isPicked()) {
                        cm->setPassengerPicked();
                        SoundManager::play(SFX::Pickup);
                        cout << "[PICKUP] " << cp->getPassengerName() 
                                  << " picked up! Time started: " << (int)cm->getTimeLeft() << "s\n";
                    }
                }

                cm->Update(dt, player);

                if (cm->isMissionCompletedFlag() && !waitingForNextMission) {
                    waitingForNextMission = true;
                    totalScore += cm->getFareReward();
                    SoundManager::play(SFX::Complete);
                    
                    // Score & Progress Save
                    int rewardAmount = cm->getFareReward(); 
                    if (!isCustomMission) {
                        int highestUnlocked = currentMissionIdx + 2; 
                        AuthManager::saveProgress(rewardAmount, highestUnlocked, totalScore);
                    }

                    if (isCustomMission)
                        cout << "[COMPLETE] Custom ride done! +" 
                                  << rewardAmount << " TK | Total Score: " << totalScore << "\n";
                    else
                        cout << "[COMPLETE] Mission " << currentMissionIdx+1 << " completed! +" 
                                  << rewardAmount << " TK | Total Score: " << totalScore << "\n";
                }
                
                if (cm->getStatus() == 3) {
                    isGameOver = true;
                    if (isCustomMission)
                        cout << "[FAILED] Custom ride failed! Game Over!\n";
                    else
                        cout << "[FAILED] Mission " << currentMissionIdx+1 << " failed! Game Over!\n";
                }
            }
        }
        // Camera
        if (!waitingForNextMission && !isGameOver)
            camera.setCenter(sf::Vector2f(player->getX(), player->getY()));
        window.setView(camera);

                      
        // DRAWING
       
        window.clear(sf::Color(34, 139, 34));

        
        bool isPlaying = (dynamic_cast<PlayingScreen*>(currentState) != nullptr);

        if (isPlaying) {
            //draw map
            env.draw(window);

            // 1. CAMPUS PERIMETER FILL
            sf::RectangleShape campusBg(sf::Vector2f(WORLD_WIDTH, WORLD_HEIGHT));
            campusBg.setFillColor(sf::Color(210, 200, 170));
            campusBg.setPosition(sf::Vector2f(0, 0));
            window.draw(campusBg);

            // 2. GRASS ZONES
            sf::RectangleShape greenZone(sf::Vector2f(395, 1500));
            greenZone.setFillColor(sf::Color(60, 160, 60, 180));
            greenZone.setPosition(sf::Vector2f(0, 0));
            window.draw(greenZone);

            sf::RectangleShape topGreen(sf::Vector2f(WORLD_WIDTH, 95));
            topGreen.setFillColor(sf::Color(60, 160, 60, 180));
            topGreen.setPosition(sf::Vector2f(0, 0));
            window.draw(topGreen);

            sf::RectangleShape rightGreen(sf::Vector2f(130, WORLD_HEIGHT));
            rightGreen.setFillColor(sf::Color(60, 160, 60, 180));
            rightGreen.setPosition(sf::Vector2f(1870, 0));
            window.draw(rightGreen);

            // 3. GARDENS
            drawGarden(window, 1455, 50, 285, 175, "Mango Garden", font);
            drawGarden(window, 1275, 330, 140, 135, "Litchi Garden", font);

            // 4. PLAY GROUNDS
            drawPlayField(window, 545, 100, 290, 230, "Play Ground", font);
            drawPlayField(window, 255, 625, 190, 235, "Play Ground", font);
            drawPlayField(window, 1425, 940, 290, 205, "Play Ground", font);

            // 5. PONDS
            for (const auto& p : ponds) {
                sf::RectangleShape water(sf::Vector2f(p.width, p.height));
                water.setFillColor(sf::Color(30, 144, 255, 195));
                water.setOutlineThickness(3.5f);
                water.setOutlineColor(sf::Color(105, 105, 105));
                water.setPosition(sf::Vector2f(p.x, p.y));
                window.draw(water);

                sf::RectangleShape shimmer(sf::Vector2f(p.width * 0.6f, 6.f));
                shimmer.setFillColor(sf::Color(180, 230, 255, 120));
                shimmer.setPosition(sf::Vector2f(p.x + p.width * 0.2f, p.y + 12.f));
                window.draw(shimmer);

                sf::Text wLbl(font, p.label, 11);
                wLbl.setFillColor(sf::Color(0, 100, 200));
                wLbl.setOutlineColor(sf::Color::White);
                wLbl.setOutlineThickness(1);
                wLbl.setPosition(sf::Vector2f(p.x + 4.f, p.y + 4.f));
                window.draw(wLbl);
            }

            // 6. ROAD NETWORK
            const sf::Color roadCol(70, 70, 70);
            const sf::Color laneCol(255, 255, 255, 160);

            auto drawHRoad = [&](float rx, float ry, float rw, float rh) {
                sf::RectangleShape r(sf::Vector2f(rw, rh));
                r.setFillColor(roadCol);
                r.setPosition(sf::Vector2f(rx, ry));
                window.draw(r);
                for (float x = rx; x < rx + rw; x += 80.f) {
                    sf::RectangleShape dash(sf::Vector2f(40.f, 4.f));
                    dash.setFillColor(laneCol);
                    dash.setPosition(sf::Vector2f(x, ry + rh * 0.5f - 2.f));
                    window.draw(dash);
                }
            };
            auto drawVRoad = [&](float rx, float ry, float rw, float rh) {
                sf::RectangleShape r(sf::Vector2f(rw, rh));
                r.setFillColor(roadCol);
                r.setPosition(sf::Vector2f(rx, ry));
                window.draw(r);
                for (float y = ry; y < ry + rh; y += 80.f) {
                    sf::RectangleShape dash(sf::Vector2f(4.f, 40.f));
                    dash.setFillColor(laneCol);
                    dash.setPosition(sf::Vector2f(rx + rw * 0.5f - 2.f, y));
                    window.draw(dash);
                }
            };

            // Roads drawn from the shared campus road list (keeps map + snapping in sync)
            for (const auto& rd : roads) {
                if (rd.isVertical)
                    drawVRoad(rd.x, rd.y, rd.width, rd.height);
                else
                    drawHRoad(rd.x, rd.y, rd.width, rd.height);
            }

            // 7. SIDEWALKS
            sf::RectangleShape sw(sf::Vector2f(WORLD_WIDTH, 16));
            sw.setFillColor(sf::Color(180, 160, 120));
            sw.setPosition(sf::Vector2f(0, 1395));
            window.draw(sw);
            sw.setPosition(sf::Vector2f(0, 1476));
            window.draw(sw);

            // 8. TREES
            for (const auto& tp : treePosns) drawTree(window, tp.x, tp.y);

            // 8b. DECORATIVE ELEMENTS
            // zebra crossing on the bottom main road
            drawZebraCrossing(window, 900, 1400, 80);

            for (const auto& lp : lampPosns) drawLamp(window, lp.x, lp.y, worldAnimTimer);
            for (const auto& bp : benchPosns) drawBench(window, bp.x, bp.y);
            for (const auto& fp : flowerPosns) drawFlowerBed(window, fp.x, fp.y, 60, 30);
            for (const auto& sp : signPosns) drawSignPost(window, sp.first.x, sp.first.y, sp.second, font);

            // 9. BUILDINGS
            for (const auto& b : buildings) {
                sf::RectangleShape body(sf::Vector2f(b.width, b.height));
                body.setFillColor(b.color);
                body.setOutlineThickness(2);
                body.setOutlineColor(sf::Color::Black);
                body.setPosition(sf::Vector2f(b.x, b.y));
                window.draw(body);

                sf::RectangleShape roof(sf::Vector2f(b.width + 10, 10));
                roof.setFillColor(b.roofColor);
                roof.setPosition(sf::Vector2f(b.x - 5, b.y - 10));
                window.draw(roof);

                if (b.hasChimney) {
                    sf::RectangleShape ch(sf::Vector2f(10, 20));
                    ch.setFillColor(sf::Color(100, 70, 40));
                    ch.setPosition(sf::Vector2f(b.x + b.width - 25, b.y - 26));
                    window.draw(ch);
                }

                int cols = max(1, (int)(b.width / 30));
                int rows = max(1, (int)(b.height / 35));
                for (int r = 0; r < rows; r++)
                    for (int c = 0; c < cols; c++) {
                        sf::RectangleShape win(sf::Vector2f(11, 14));
                        win.setFillColor(sf::Color(255, 255, 150));
                        win.setOutlineThickness(1);
                        win.setOutlineColor(sf::Color::Black);
                        win.setPosition(sf::Vector2f(b.x + 8 + c * 28, b.y + 14 + r * 33));
                        window.draw(win);
                    }

                sf::Text lbl(font, b.name, 16);
                lbl.setFillColor(sf::Color::White);
                lbl.setOutlineThickness(1.5f);
                lbl.setOutlineColor(sf::Color::Black);
                lbl.setPosition(sf::Vector2f(b.x + 3, b.y + b.height - 23));
                lbl.setStyle(sf::Text::Bold);
                window.draw(lbl);
            }

            // 10. SHAHEED MINAR DETAIL
            {
                sf::RectangleShape minar(sf::Vector2f(8, 80));
                minar.setFillColor(sf::Color(230, 230, 250));
                minar.setOutlineThickness(1);
                minar.setOutlineColor(sf::Color(100, 100, 150));
                minar.setPosition(sf::Vector2f(1413, 1090));
                window.draw(minar);
                sf::CircleShape cap(10);
                cap.setFillColor(sf::Color(200, 200, 240));
                cap.setPosition(sf::Vector2f(1403, 1085));
                window.draw(cap);
            }

            // 11. OBSTACLES
            for (const auto& obs : obstacles) {
                sf::RectangleShape shape;
                if (obs.type == "pothole") {
                    shape.setSize(sf::Vector2f(32, 18));
                    shape.setFillColor(sf::Color(20, 20, 20));
                    shape.setOutlineThickness(1.5f);
                    shape.setOutlineColor(sf::Color(90, 90, 90));
                } else {
                    shape.setSize(sf::Vector2f(42, 15));
                    shape.setFillColor(sf::Color(255, 200, 0));
                    shape.setOutlineThickness(2.f);
                    shape.setOutlineColor(sf::Color::Black);
                }
                shape.setPosition(sf::Vector2f(obs.x - 16, obs.y - 8));
                window.draw(shape);

                if (obs.type == "speed_breaker") {
                    for (int i = 0; i < 3; i++) {
                        sf::RectangleShape stripe(sf::Vector2f(8, 15));
                        stripe.setFillColor(sf::Color::Black);
                        stripe.setPosition(sf::Vector2f(obs.x - 10 + i * 14, obs.y - 8));
                        window.draw(stripe);
                    }
                    sf::Text bt(font, "BUMP", 8);
                    bt.setFillColor(sf::Color::Yellow);
                    bt.setOutlineColor(sf::Color::Black);
                    bt.setOutlineThickness(1);
                    bt.setPosition(sf::Vector2f(obs.x - 14, obs.y - 18));
                    window.draw(bt);
                }
            }

            // 12. PASSENGER
            if (activePassenger && !waitingForNextMission && !isGameOver && !activePassenger->isPicked()) {
                activePassenger->draw(window);
                sf::Text pn(font, activePassenger->getPassengerName(), 11);
                pn.setFillColor(sf::Color::Cyan);
                pn.setOutlineColor(sf::Color::Black);
                pn.setOutlineThickness(1);
                pn.setPosition(sf::Vector2f(activePassenger->getX() - 20, activePassenger->getY() - 48));
                window.draw(pn);

                // Friendly pickup hint when the rickshaw is in range
                if (activePassenger->isNear()) {
                    sf::Text pickupHint(font, "PRESS SPACE TO PICK UP", 14);
                    pickupHint.setFillColor(sf::Color(80, 255, 120));
                    pickupHint.setOutlineColor(sf::Color::Black);
                    pickupHint.setOutlineThickness(2);
                    sf::FloatRect hb = pickupHint.getLocalBounds();
                    pickupHint.setOrigin(sf::Vector2f(hb.position.x + hb.size.x / 2.f, hb.position.y + hb.size.y / 2.f));
                    pickupHint.setPosition(sf::Vector2f(activePassenger->getX(), activePassenger->getY() - 66.f));
                    window.draw(pickupHint);
                }
            }

            // 13. DESTINATION MARKER
            if (activeMission && !waitingForNextMission && !isGameOver && activeMission->isPassengerPicked() && activeMission->getStatus() == 1) {
                float destX = activeMission->getTargetX();
                float destY = activeMission->getTargetY();

                static float pulse = 0;
                pulse += 0.1f;
                float radius = 20.0f + sin(pulse) * 5.0f;

                sf::CircleShape greenSignal(radius);
                greenSignal.setFillColor(sf::Color(0, 255, 0, 100));
                greenSignal.setOutlineThickness(3);
                greenSignal.setOutlineColor(sf::Color::Green);
                greenSignal.setOrigin(sf::Vector2f(radius, radius));
                greenSignal.setPosition(sf::Vector2f(destX, destY));
                window.draw(greenSignal);

                sf::Text dropText(font, "DROP HERE", 10);
                dropText.setFillColor(sf::Color::Green);
                dropText.setOutlineThickness(1);
                dropText.setOutlineColor(sf::Color::Black);
                dropText.setPosition(sf::Vector2f(destX - 25, destY - 30));
                window.draw(dropText);

                float ddx = player->getX() - destX;
                float ddy = player->getY() - destY;
                float dist = sqrt(ddx*ddx + ddy*ddy);
                sf::Text dt_(font, to_string((int)dist) + " m", 13);
                dt_.setFillColor(sf::Color::Yellow);
                dt_.setOutlineColor(sf::Color::Black);
                dt_.setOutlineThickness(1);
                dt_.setPosition(sf::Vector2f(destX + 26, destY - 12));
                window.draw(dt_);
            }

            // 14. PLAYER
            player->draw(window);

            // 15. MINIMAP
            minimap.draw(window, player->getX(), player->getY(), 0);

            // 16. HUD 
            window.setView(window.getDefaultView());

            auto drawBar = [&](float x, float y, float w, float h, float fraction, sf::Color fill) {
                if (fraction < 0.f) fraction = 0.f;
                if (fraction > 1.f) fraction = 1.f;
                sf::RectangleShape bg(sf::Vector2f(w, h));
                bg.setFillColor(sf::Color(15, 20, 30, 200));
                bg.setOutlineThickness(1.5f);
                bg.setOutlineColor(sf::Color(120, 135, 155));
                bg.setPosition(sf::Vector2f(x, y));
                window.draw(bg);
                if (fraction > 0.01f) {
                    sf::RectangleShape fg(sf::Vector2f(w * fraction, h));
                    fg.setFillColor(fill);
                    sf::RectangleShape fgOutline(sf::Vector2f(w * fraction, h));
                    fgOutline.setFillColor(sf::Color::Transparent);
                    fgOutline.setOutlineThickness(1.f);
                    fgOutline.setOutlineColor(sf::Color::White);
                    fgOutline.setPosition(sf::Vector2f(x, y));
                    fg.setPosition(sf::Vector2f(x, y));
                    window.draw(fg);
                    window.draw(fgOutline);
                }
            };

            if (isGameOver) {
                // GAME OVER overlay
                sf::RectangleShape veil(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                veil.setFillColor(sf::Color(5, 5, 10, 200));
                window.draw(veil);

                sf::RectangleShape goPanel(sf::Vector2f(520.f, 320.f));
                goPanel.setFillColor(sf::Color(25, 15, 15, 240));
                goPanel.setOutlineThickness(3.f);
                goPanel.setOutlineColor(sf::Color(230, 60, 60));
                goPanel.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 260.f, WINDOW_HEIGHT/2 - 160.f));
                window.draw(goPanel);

                sf::RectangleShape goAccent(sf::Vector2f(520.f, 4.f));
                goAccent.setFillColor(sf::Color(230, 60, 60));
                goAccent.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 260.f, WINDOW_HEIGHT/2 - 160.f));
                window.draw(goAccent);

                sf::Text goTitle(font, "GAME OVER", 44);
                goTitle.setStyle(sf::Text::Bold);
                goTitle.setFillColor(sf::Color(230, 60, 60));
                sf::FloatRect gtb = goTitle.getLocalBounds();
                goTitle.setOrigin(sf::Vector2f(gtb.position.x + gtb.size.x/2.f, gtb.position.y + gtb.size.y/2.f));
                goTitle.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 95.f));
                window.draw(goTitle);

                sf::Text goScore(font, "Final Score: " + to_string(totalScore), 24);
                goScore.setFillColor(sf::Color(255, 200, 0));
                sf::FloatRect gsb = goScore.getLocalBounds();
                goScore.setOrigin(sf::Vector2f(gsb.position.x + gsb.size.x/2.f, gsb.position.y + gsb.size.y/2.f));
                goScore.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 20.f));
                window.draw(goScore);

                sf::Text goDur(font, isCustomMission ? "Custom Ride Failed" : ("Missions Completed: " + to_string(currentMissionIdx) + " / 5"), 18);
                goDur.setFillColor(sf::Color(200, 210, 225));
                sf::FloatRect gdb = goDur.getLocalBounds();
                goDur.setOrigin(sf::Vector2f(gdb.position.x + gdb.size.x/2.f, gdb.position.y + gdb.size.y/2.f));
                goDur.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 20.f));
                window.draw(goDur);

                sf::Text goHint(font, "Press R to retry mission   |   ESC to exit", 15);
                goHint.setFillColor(sf::Color(160, 175, 195));
                sf::FloatRect ghb = goHint.getLocalBounds();
                goHint.setOrigin(sf::Vector2f(ghb.position.x + ghb.size.x/2.f, ghb.position.y + ghb.size.y/2.f));
                goHint.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 75.f));
                window.draw(goHint);

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
                    // Retry current mission
                    isGameOver = false;
                    waitingForNextMission = false;
                    Mechanic::repair(*player);
                    if (isCustomMission) {
                        if (customMission) customMission->Start(player);
                        if (customPassenger) customPassenger->reset();
                    } else {
                        missions[currentMissionIdx]->Start(player);
                        passengers[currentMissionIdx]->reset();
                    }
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                    window.close();
                }
            } else if (waitingForNextMission) {
                bool customDone = isCustomMission;
                string next = (currentMissionIdx + 1 < (int)missions.size()) ? missions[currentMissionIdx + 1]->getPassengerName() : "All Missions Complete!";
                bool allDone = (currentMissionIdx + 1 >= (int)missions.size());
                sf::RectangleShape veil(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
                veil.setFillColor(sf::Color(5, 10, 20, 170));
                window.draw(veil);

                sf::RectangleShape cmp(sf::Vector2f(560.f, 300.f));
                cmp.setFillColor(sf::Color(15, 40, 25, 240));
                cmp.setOutlineThickness(3.f);
                cmp.setOutlineColor(sf::Color(0, 255, 150));
                cmp.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 280.f, WINDOW_HEIGHT/2 - 150.f));
                window.draw(cmp);

                sf::RectangleShape cmpAccent(sf::Vector2f(560.f, 4.f));
                cmpAccent.setFillColor(sf::Color(0, 255, 150));
                cmpAccent.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 280.f, WINDOW_HEIGHT/2 - 150.f));
                window.draw(cmpAccent);

                sf::Text cmpTitle(font, customDone ? "CUSTOM RIDE COMPLETE!" : "MISSION COMPLETE!", 36);
                cmpTitle.setStyle(sf::Text::Bold);
                cmpTitle.setFillColor(sf::Color(0, 255, 150));
                sf::FloatRect ctb = cmpTitle.getLocalBounds();
                cmpTitle.setOrigin(sf::Vector2f(ctb.position.x + ctb.size.x/2.f, ctb.position.y + ctb.size.y/2.f));
                cmpTitle.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 105.f));
                window.draw(cmpTitle);

                sf::Text cmpScore(font, "+ " + to_string(activeMission->getFareReward()) + " TK", 30);
                cmpScore.setStyle(sf::Text::Bold);
                cmpScore.setFillColor(sf::Color(255, 200, 0));
                sf::FloatRect csb = cmpScore.getLocalBounds();
                cmpScore.setOrigin(sf::Vector2f(csb.position.x + csb.size.x/2.f, csb.position.y + csb.size.y/2.f));
                cmpScore.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 20.f));
                window.draw(cmpScore);

                sf::Text cmpTotal(font, "Total Score: " + to_string(totalScore), 20);
                cmpTotal.setFillColor(sf::Color(200, 210, 225));
                sf::FloatRect cdb = cmpTotal.getLocalBounds();
                cmpTotal.setOrigin(sf::Vector2f(cdb.position.x + cdb.size.x/2.f, cdb.position.y + cdb.size.y/2.f));
                cmpTotal.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 25.f));
                window.draw(cmpTotal);

                if (customDone) {
                    sf::Text cmpNext(font, "Custom Rider delivered!", 16);
                    cmpNext.setFillColor(sf::Color(150, 170, 190));
                    sf::FloatRect cnb = cmpNext.getLocalBounds();
                    cmpNext.setOrigin(sf::Vector2f(cnb.position.x + cnb.size.x/2.f, cnb.position.y + cnb.size.y/2.f));
                    cmpNext.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 70.f));
                    window.draw(cmpNext);
                } else {
                    sf::Text cmpNext(font, "Next: " + next, 16);
                    cmpNext.setFillColor(sf::Color(150, 170, 190));
                    sf::FloatRect cnb = cmpNext.getLocalBounds();
                    cmpNext.setOrigin(sf::Vector2f(cnb.position.x + cnb.size.x/2.f, cnb.position.y + cnb.size.y/2.f));
                    cmpNext.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 70.f));
                    window.draw(cmpNext);
                }

                sf::Text cmpHint(font, allDone && !customDone ? "Press ESC to exit" : "Press ENTER to continue", 16);
                cmpHint.setStyle(sf::Text::Bold);
                cmpHint.setFillColor(sf::Color(255, 255, 255));
                cmpHint.setOutlineThickness(1.f);
                cmpHint.setOutlineColor(sf::Color::Black);
                sf::FloatRect chb = cmpHint.getLocalBounds();
                cmpHint.setOrigin(sf::Vector2f(chb.position.x + chb.size.x/2.f, chb.position.y + chb.size.y/2.f));
                cmpHint.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 110.f));
                window.draw(cmpHint);
            } else {
                // === TOP-LEFT: Stats panel ===
                float fuelFrac = player->getFuel() / 100.f;
                float healthFrac = player->getHealth() / 100.f;
                sf::Color fuelColor = fuelFrac > 0.5f ? sf::Color(0, 200, 120) : (fuelFrac > 0.25f ? sf::Color(255, 180, 0) : sf::Color(230, 60, 60));
                sf::Color healthColor = healthFrac > 0.5f ? sf::Color(0, 200, 255) : (healthFrac > 0.25f ? sf::Color(255, 180, 0) : sf::Color(230, 60, 60));

                sf::RectangleShape hudPanel(sf::Vector2f(420.f, 150.f));
                hudPanel.setFillColor(sf::Color(15, 20, 30, 215));
                hudPanel.setOutlineThickness(1.5f);
                hudPanel.setOutlineColor(sf::Color(90, 105, 125));
                hudPanel.setPosition(sf::Vector2f(15.f, 10.f));
                window.draw(hudPanel);

                sf::RectangleShape hudAccent(sf::Vector2f(420.f, 3.f));
                hudAccent.setFillColor(sf::Color(0, 160, 255));
                hudAccent.setPosition(sf::Vector2f(15.f, 10.f));
                window.draw(hudAccent);

                sf::Text mNum(font, isCustomMission ? "CUSTOM MISSION" : ("MISSION " + to_string(currentMissionIdx + 1) + " / 5"), 17);
                mNum.setStyle(sf::Text::Bold);
                mNum.setFillColor(sf::Color(255, 200, 0));
                mNum.setPosition(sf::Vector2f(30.f, 18.f));
                window.draw(mNum);

                sf::Text scoreTxt(font, "SCORE  " + to_string(totalScore), 15);
                scoreTxt.setStyle(sf::Text::Bold);
                scoreTxt.setFillColor(sf::Color(255, 255, 255));
                scoreTxt.setPosition(sf::Vector2f(300.f, 20.f));
                window.draw(scoreTxt);

                // Fuel bar
                sf::Text fuelLbl(font, "FUEL", 13);
                fuelLbl.setFillColor(sf::Color(200, 210, 225));
                fuelLbl.setPosition(sf::Vector2f(30.f, 52.f));
                window.draw(fuelLbl);
                drawBar(95.f, 55.f, 220.f, 16.f, fuelFrac, fuelColor);
                sf::Text fuelPct(font, to_string((int)player->getFuel()) + "%", 13);
                fuelPct.setFillColor(fuelColor);
                fuelPct.setPosition(sf::Vector2f(330.f, 52.f));
                window.draw(fuelPct);

                // Health bar
                sf::Text hpLbl(font, "HULL", 13);
                hpLbl.setFillColor(sf::Color(200, 210, 225));
                hpLbl.setPosition(sf::Vector2f(30.f, 84.f));
                window.draw(hpLbl);
                drawBar(95.f, 87.f, 220.f, 16.f, healthFrac, healthColor);
                sf::Text hpPct(font, to_string(player->getHealth()) + "%", 13);
                hpPct.setFillColor(healthColor);
                hpPct.setPosition(sf::Vector2f(330.f, 84.f));
                window.draw(hpPct);

                // Timer (if picked up)
                if (activeMission->isPassengerPicked()) {
                    float timeFrac = activeMission->getTimeLeft() / activeMission->getTimeLimit();
                    sf::Text timeLbl(font, "TIME", 13);
                    timeLbl.setFillColor(sf::Color(200, 210, 225));
                    timeLbl.setPosition(sf::Vector2f(30.f, 118.f));
                    window.draw(timeLbl);
                    drawBar(95.f, 121.f, 220.f, 16.f, timeFrac, timeFrac > 0.3f ? sf::Color(255, 150, 0) : sf::Color(230, 60, 60));
                    sf::Text timePct(font, to_string((int)activeMission->getTimeLeft()) + "s", 13);
                    timePct.setFillColor(sf::Color(255, 200, 100));
                    timePct.setPosition(sf::Vector2f(330.f, 118.f));
                    window.draw(timePct);
                }

                // === Dialogue / objective box ===
                sf::RectangleShape objPanel(sf::Vector2f(600.f, 70.f));
                objPanel.setFillColor(sf::Color(15, 20, 30, 215));
                objPanel.setOutlineThickness(1.5f);
                objPanel.setOutlineColor(sf::Color(90, 105, 125));
                objPanel.setPosition(sf::Vector2f(15.f, 175.f));
                window.draw(objPanel);

                sf::RectangleShape objAccent(sf::Vector2f(600.f, 3.f));
                objAccent.setFillColor(sf::Color(0, 255, 150));
                objAccent.setPosition(sf::Vector2f(15.f, 175.f));
                window.draw(objAccent);

                sf::Text obj(font, activeMission->getDialogue(), 15);
                obj.setFillColor(sf::Color(235, 240, 250));
                obj.setPosition(sf::Vector2f(30.f, 183.f));
                window.draw(obj);

                // === Destination highlighter on the player's view ===
                if (activeMission->isPassengerPicked() && activeMission->getStatus() == 1) {
                    float destX = activeMission->getTargetX();
                    float destY = activeMission->getTargetY();
                    bool isPlayingNow = (dynamic_cast<PlayingScreen*>(currentState) != nullptr);
                    if (isPlayingNow) {
                        // World -> screen position using the current camera
                        sf::Vector2f camC = camera.getCenter();
                        sf::Vector2f camS = camera.getSize();
                        float sx = (destX - camC.x) * (WINDOW_WIDTH / camS.x) + WINDOW_WIDTH / 2.f;
                        float sy = (destY - camC.y) * (WINDOW_HEIGHT / camS.y) + WINDOW_HEIGHT / 2.f;

                        float m = 70.f; // safe margin from screen edges
                        bool onScreen = (sx >= m && sx <= WINDOW_WIDTH - m &&
                                         sy >= m && sy <= WINDOW_HEIGHT - m);

                        float pTime = worldAnimTimer;
                        float pDist = std::sqrt((player->getX()-destX)*(player->getX()-destX) +
                                                (player->getY()-destY)*(player->getY()-destY));

                        if (onScreen) {
                            // Pulsing gold diamond beacon in the player's view at the destination
                            float r = 15.f + std::sin(pTime * 5.f) * 4.f;
                            sf::ConvexShape diamond(4);
                            diamond.setPoint(0, sf::Vector2f(0.f, -r));
                            diamond.setPoint(1, sf::Vector2f(r, 0.f));
                            diamond.setPoint(2, sf::Vector2f(0.f, r));
                            diamond.setPoint(3, sf::Vector2f(-r, 0.f));
                            diamond.setFillColor(sf::Color(255, 205, 0, 190));
                            diamond.setOutlineThickness(3.f);
                            diamond.setOutlineColor(sf::Color(255, 110, 0));
                            diamond.setPosition(sf::Vector2f(sx, sy));
                            window.draw(diamond);

                            sf::Text destLbl(font, "DROP POINT", 11);
                            destLbl.setFillColor(sf::Color(255, 205, 0));
                            destLbl.setOutlineColor(sf::Color::Black);
                            destLbl.setOutlineThickness(1.5f);
                            sf::FloatRect dlb = destLbl.getLocalBounds();
                            destLbl.setOrigin(sf::Vector2f(dlb.position.x + dlb.size.x/2.f, dlb.position.y + dlb.size.y/2.f));
                            destLbl.setPosition(sf::Vector2f(sx, sy + r + 12.f));
                            window.draw(destLbl);
                        } else {
                            // Off-screen compass arrow pointing to the destination
                            sf::Vector2f center(WINDOW_WIDTH/2.f, WINDOW_HEIGHT/2.f);
                            float dx = sx - center.x;
                            float dy = sy - center.y;
                            float ang = std::atan2(dy, dx);

                            // Intersect the direction ray with the safe screen box
                            float tBest = 1e9f;
                            sf::Vector2f edge = center;
                            auto candidate = [&](float t, float ex, float ey) {
                                if (t > 0.001f && t < tBest) { tBest = t; edge = sf::Vector2f(ex, ey); }
                            };
                            if (std::fabs(dx) > 0.001f) {
                                candidate((m - center.x) / dx, m, center.y + ((m - center.x)/dx) * dy);
                                candidate((WINDOW_WIDTH - m - center.x) / dx, WINDOW_WIDTH - m, center.y + ((WINDOW_WIDTH - m - center.x)/dx) * dy);
                            }
                            if (std::fabs(dy) > 0.001f) {
                                candidate((m - center.y) / dy, center.x + ((m - center.y)/dy) * dx, m);
                                candidate((WINDOW_HEIGHT - m - center.y) / dy, center.x + ((WINDOW_HEIGHT - m - center.y)/dy) * dx, WINDOW_HEIGHT - m);
                            }
                            edge.x = std::max(m, std::min(WINDOW_WIDTH - m, edge.x));
                            edge.y = std::max(m, std::min(WINDOW_HEIGHT - m, edge.y));

                            // Arrow graphic pointing +x, rotated toward the destination
                            sf::ConvexShape arrow(7);
                            arrow.setPoint(0, sf::Vector2f(-13.f, -18.f));
                            arrow.setPoint(1, sf::Vector2f(-13.f, -7.f));
                            arrow.setPoint(2, sf::Vector2f(-28.f, -7.f));
                            arrow.setPoint(3, sf::Vector2f(-28.f, 7.f));
                            arrow.setPoint(4, sf::Vector2f(-13.f, 7.f));
                            arrow.setPoint(5, sf::Vector2f(-13.f, 18.f));
                            arrow.setPoint(6, sf::Vector2f(15.f, 0.f));
                            arrow.setFillColor(sf::Color(255, 200, 0));
                            arrow.setOutlineThickness(2.f);
                            arrow.setOutlineColor(sf::Color(20, 10, 0));
                            arrow.setRotation(sf::radians(ang));
                            arrow.setPosition(edge);
                            window.draw(arrow);

                            // Distance + direction label next to the arrow
                            std::string dir;
                            if (ang > -0.7854f && ang <= 0.7854f) dir = "EAST";
                            else if (ang > 0.7854f && ang <= 2.3562f) dir = "SOUTH";
                            else if (ang > -2.3562f && ang <= -0.7854f) dir = "NORTH";
                            else dir = "WEST";
                            sf::Text destLbl(font, dir + "  " + std::to_string((int)pDist) + " m", 14);
                            destLbl.setStyle(sf::Text::Bold);
                            destLbl.setFillColor(sf::Color(255, 205, 0));
                            destLbl.setOutlineColor(sf::Color::Black);
                            destLbl.setOutlineThickness(2.f);
                            sf::FloatRect dlb = destLbl.getLocalBounds();
                            destLbl.setOrigin(sf::Vector2f(dlb.position.x + dlb.size.x/2.f, dlb.position.y + dlb.size.y/2.f));
                            float len = std::sqrt(dx*dx + dy*dy) + 0.0001f;
                            sf::Vector2f off(dx / len, dy / len);
                            destLbl.setPosition(sf::Vector2f(edge.x + off.x * 34.f, edge.y + off.y * 34.f));
                            window.draw(destLbl);
                        }
                    }
                }

                // === Controls hint bottom center ===
                sf::Text ctrl(font, "WASD Move | SHIFT Boost | SPACE Pickup | R Repair | U Time | P Pause | H Help", 14);
                ctrl.setFillColor(sf::Color(200, 210, 225, 200));
                sf::FloatRect ctrlb = ctrl.getLocalBounds();
                ctrl.setOrigin(sf::Vector2f(ctrlb.position.x + ctrlb.size.x/2.f, ctrlb.position.y + ctrlb.size.y/2.f));
                ctrl.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT - 25.f));
                window.draw(ctrl);
            }
        } 
        else {
           window.setView(window.getDefaultView()); 
            
            // ui draw hbe
            currentState->render(window, font);
        }

        // === PAUSE MENU OVERLAY ===
        if (paused && isPlaying && !isGameOver && !waitingForNextMission) {
            window.setView(window.getDefaultView());

            sf::RectangleShape veil(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
            veil.setFillColor(sf::Color(5, 8, 15, 190));
            window.draw(veil);

            sf::RectangleShape pPanel(sf::Vector2f(560.f, 350.f));
            pPanel.setFillColor(sf::Color(18, 24, 40, 248));
            pPanel.setOutlineThickness(3.f);
            pPanel.setOutlineColor(sf::Color(0, 170, 255));
            pPanel.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 280.f, WINDOW_HEIGHT/2 - 175.f));
            window.draw(pPanel);

            sf::RectangleShape pAccent(sf::Vector2f(560.f, 4.f));
            pAccent.setFillColor(sf::Color(0, 170, 255));
            pAccent.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 280.f, WINDOW_HEIGHT/2 - 175.f));
            window.draw(pAccent);

            sf::Text pTitle(font, "PAUSED", 40);
            pTitle.setStyle(sf::Text::Bold);
            pTitle.setFillColor(sf::Color(0, 200, 255));
            sf::FloatRect ptb = pTitle.getLocalBounds();
            pTitle.setOrigin(sf::Vector2f(ptb.position.x + ptb.size.x/2.f, ptb.position.y + ptb.size.y/2.f));
            pTitle.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 130.f));
            window.draw(pTitle);

            struct MenuRow { std::string label; std::string key; };
            MenuRow rows[4] = {
                {"Resume game (Return)",                 "P  /  ESC"},
                {"Back to Dashboard",                    "B"},
                {"Show Controls / Help",                 "H"},
                {"Quit game",                            "Q"}
            };
            float ry = WINDOW_HEIGHT/2 - 55.f;
            for (int i = 0; i < 4; i++) {
                sf::Text lbl(font, rows[i].label, 18);
                lbl.setFillColor(sf::Color(220, 230, 245));
                lbl.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 220.f, ry));
                window.draw(lbl);

                sf::Text keyTxt(font, rows[i].key, 18);
                keyTxt.setStyle(sf::Text::Bold);
                keyTxt.setFillColor(sf::Color(255, 200, 0));
                sf::FloatRect ktb = keyTxt.getLocalBounds();
                keyTxt.setPosition(sf::Vector2f(WINDOW_WIDTH/2 + 220.f - (ktb.position.x + ktb.size.x), ry));
                window.draw(keyTxt);
                ry += 52.f;
            }

            sf::Text pFoot(font, "Press P or ESC anytime during a mission to pause and open this menu.", 13);
            pFoot.setFillColor(sf::Color(150, 165, 185));
            sf::FloatRect pfb = pFoot.getLocalBounds();
            pFoot.setOrigin(sf::Vector2f(pfb.position.x + pfb.size.x/2.f, pfb.position.y + pfb.size.y/2.f));
            pFoot.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 145.f));
            window.draw(pFoot);
        }

        // === HELP / CONTROLS OVERLAY ===
        if (helpOpen && isPlaying && !isGameOver && !waitingForNextMission) {
            window.setView(window.getDefaultView());

            sf::RectangleShape veil(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
            veil.setFillColor(sf::Color(5, 8, 15, 200));
            window.draw(veil);

            sf::RectangleShape hPanel(sf::Vector2f(620.f, 520.f));
            hPanel.setFillColor(sf::Color(18, 28, 20, 250));
            hPanel.setOutlineThickness(3.f);
            hPanel.setOutlineColor(sf::Color(0, 255, 150));
            hPanel.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 310.f, WINDOW_HEIGHT/2 - 260.f));
            window.draw(hPanel);

            sf::Text hTitle(font, "CONTROLS & INSTRUCTIONS", 24);
            hTitle.setStyle(sf::Text::Bold);
            hTitle.setFillColor(sf::Color(0, 255, 150));
            sf::FloatRect htb = hTitle.getLocalBounds();
            hTitle.setOrigin(sf::Vector2f(htb.position.x + htb.size.x/2.f, htb.position.y + htb.size.y/2.f));
            hTitle.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 220.f));
            window.draw(hTitle);

            const char* helpLines[] = {
                "WASD / Arrow Keys  -  Move the rickshaw",
                "SHIFT              -  Boost (faster, more fuel)",
                "SPACE              -  Pick up / drop off passenger",
                "P  / ESC           -  Pause menu  (Resume / Back / Quit)",
                "B                  -  Back to Dashboard (from pause)",
                "H                  -  Show / hide this help",
                "R                  -  Repair rickshaw (fuel + hull)",
                "U                  -  Change time of day",
                "1-5 (Dashboard)    -  Choose a mission",
                "U  /  G (Dashboard)-  Open the Garage / Upgrades shop",
                "C  (Dashboard)     -  Create a CUSTOM mission (pick your own route)",
                "FOLLOW THE GOLD    -  Arrow + DROP POINT shows the destination",
                "Green beacon       -  Location of the waiting passenger",
            };
            float hy = WINDOW_HEIGHT/2 - 160.f;
            for (const char* line : helpLines) {
                sf::Text t(font, line, 16);
                t.setFillColor(sf::Color(225, 235, 245));
                t.setPosition(sf::Vector2f(WINDOW_WIDTH/2 - 280.f, hy));
                window.draw(t);
                hy += 30.f;
            }
            sf::Text hFoot(font, "Press H to close this window", 14);
            hFoot.setFillColor(sf::Color(150, 210, 170));
            sf::FloatRect hfb = hFoot.getLocalBounds();
            hFoot.setOrigin(sf::Vector2f(hfb.position.x + hfb.size.x/2.f, hfb.position.y + hfb.size.y/2.f));
            hFoot.setPosition(sf::Vector2f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 + 235.f));
            window.draw(hFoot);
        }

        // Stop engine/wind when not actively driving a mission
        if (!isPlaying || paused || isGameOver || waitingForNextMission) {
            SoundManager::stopEngine();
            SoundManager::stopWind();
        }

        window.display();
    }  

    // Cleanup
    cout << "\n========== GAME SHUTDOWN ==========\n";
    cout << "Final Score: " << totalScore << "\n";
    cout << "Cleaning up resources...\n";
    
    delete player;
    delete currentState;
    for (auto m : missions) delete m;
    for (auto p : passengers) delete p;
    delete customPassenger;
    delete customMission;
    
    cout << "[OK] Cleanup complete. Goodbye!\n";
    return 0;
}  