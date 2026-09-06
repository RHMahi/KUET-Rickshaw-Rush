#include "../include/Auth.h"
#include "../include/Campus.h"
#include "../include/SoundManager.h"
#include <cmath>
#include <bits/stdc++.h>
#include <cstdint>
using namespace std;
namespace KUET {

    // ===================== Binary Encoding / Decoding =====================

    static string stringToBinary(const string& text) {
        string bin;
        for (char c : text) bin += bitset<8>(c).to_string();
        return bin;
    }

    static string binaryToString(const string& bin) {
        string text;
        for (size_t i = 0; i < bin.length(); i += 8) {
            if (i + 8 <= bin.length())
                text += char(bitset<8>(bin.substr(i, 8)).to_ulong());
        }
        return text;
    }

    // ===================== UserProfile Implementations =====================

    UserProfile::UserProfile()
        : totalEarnings(0), unlockedMission(1), highestScore(0),
          speedUpgrade(0), fuelUpgrade(0), armorUpgrade(0), turboUpgrade(0), paintIndex(0) {}
    UserProfile::UserProfile(string u, string p, int e, int m, int s, int spd, int f, int ar, int tb, int pnt)
        : username(u), password(p), totalEarnings(e), unlockedMission(m), highestScore(s),
          speedUpgrade(spd), fuelUpgrade(f), armorUpgrade(ar), turboUpgrade(tb), paintIndex(pnt) {}

    string UserProfile::getUsername() const { return username; }
    string UserProfile::getPassword() const { return password; }
    int UserProfile::getTotalEarnings() const { return totalEarnings; }
    int UserProfile::getUnlockedMission() const { return unlockedMission; }
    int UserProfile::getHighestScore() const { return highestScore; }

    int UserProfile::getSpeedUpgrade() const { return speedUpgrade; }
    int UserProfile::getFuelUpgrade() const { return fuelUpgrade; }
    int UserProfile::getArmorUpgrade() const { return armorUpgrade; }
    int UserProfile::getTurboUpgrade() const { return turboUpgrade; }
    int UserProfile::getPaintIndex() const { return paintIndex; }

    void UserProfile::addEarnings(int amount) { if (amount > 0) totalEarnings += amount; }
    void UserProfile::spendEarnings(int amount) {
        if (amount > 0) totalEarnings -= amount;
        if (totalEarnings < 0) totalEarnings = 0;
    }
    void UserProfile::updateUnlockedMission(int m) { if (m > unlockedMission) unlockedMission = m; }
    void UserProfile::updateHighestScore(int score) { if (score > highestScore) highestScore = score; }

    void UserProfile::setSpeedUpgrade(int v) { speedUpgrade = v < 0 ? 0 : (v > 3 ? 3 : v); }
    void UserProfile::setFuelUpgrade(int v) { fuelUpgrade = v < 0 ? 0 : (v > 3 ? 3 : v); }
    void UserProfile::setArmorUpgrade(int v) { armorUpgrade = v < 0 ? 0 : (v > 3 ? 3 : v); }
    void UserProfile::setTurboUpgrade(int v) { turboUpgrade = v ? 1 : 0; }
    void UserProfile::setPaintIndex(int v) { paintIndex = v < 0 ? 0 : (v > 4 ? 4 : v); }

    // ===================== AuthManager Implementations =====================

    UserProfile AuthManager::currentUser;
    bool AuthManager::isLoggedIn = false;
    const string FILE_NAME = "users.txt";

    bool AuthManager::checkIsLoggedIn() { return isLoggedIn; }
    UserProfile& AuthManager::getCurrentUser() { return currentUser; }

    void AuthManager::logout() {
        isLoggedIn = false;
        currentUser = UserProfile();
    }

    bool AuthManager::loginUser(const string& username, const string& password) {
        ifstream file(FILE_NAME);
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            istringstream iss(line);
            string u, binPass;
            int e = 0, m = 1, s = 0;
            int spd = 0, fuel = 0, ar = 0, tb = 0, pnt = 0;
            if (!(iss >> u >> binPass)) continue;
            iss >> e >> m >> s;
            // Extended upgrade fields (backward compatible: default 0 if absent)
            iss >> spd >> fuel >> ar >> tb >> pnt;
            if (u == username && binaryToString(binPass) == password) {
                currentUser = UserProfile(u, password, e, m, s, spd, fuel, ar, tb, pnt);
                isLoggedIn = true;
                return true;
            }
        }
        return false;
    }

    bool AuthManager::registerUser(const string& username, const string& password) {
        ifstream fileIn(FILE_NAME);
        string line;
        while (getline(fileIn, line)) {
            if (line.empty()) continue;
            istringstream iss(line);
            string u, p;
            if ((iss >> u >> p) && u == username) return false;
        }
        ofstream fileOut(FILE_NAME, ios::app);
        fileOut << username << " " << stringToBinary(password) << " 0 1 0 0 0 0 0 0\n";
        return true;
    }

    void AuthManager::saveProgress(int extraEarnings, int highestMissionCompleted, int currentScore) {
        if (!isLoggedIn) return;
        currentUser.addEarnings(extraEarnings);
        currentUser.updateUnlockedMission(highestMissionCompleted);
        currentUser.updateHighestScore(currentScore);

        vector<UserProfile> users;
        ifstream fileIn(FILE_NAME);
        string line;
        while (getline(fileIn, line)) {
            if (line.empty()) continue;
            istringstream iss(line);
            string u, binPass;
            int e = 0, m = 1, s = 0;
            int spd = 0, fuel = 0, ar = 0, tb = 0, pnt = 0;
            if (!(iss >> u >> binPass)) continue;
            iss >> e >> m >> s;
            iss >> spd >> fuel >> ar >> tb >> pnt;
            if (u == currentUser.getUsername()) users.push_back(currentUser);
            else users.push_back(UserProfile(u, binaryToString(binPass), e, m, s, spd, fuel, ar, tb, pnt));
        }
        ofstream fileOut(FILE_NAME);
        for (const auto& user : users) {
            fileOut << user.getUsername() << " " << stringToBinary(user.getPassword())
                    << " " << user.getTotalEarnings() << " " << user.getUnlockedMission()
                    << " " << user.getHighestScore()
                    << " " << user.getSpeedUpgrade() << " " << user.getFuelUpgrade()
                    << " " << user.getArmorUpgrade() << " " << user.getTurboUpgrade()
                    << " " << user.getPaintIndex() << "\n";
        }
    }

    // Persist current upgrades after a purchase in the garage
    void AuthManager::saveUpgrades() {
        if (!isLoggedIn) return;
        saveProgress(0, currentUser.getUnlockedMission(), currentUser.getHighestScore());
    }

    // ===================== Shared UI Helpers =====================

    // Neon cyberpunk palette (matches background.png night-scene colors)
    namespace UIPalette {
        const sf::Color DeepBg   (13, 35, 47);   // deep teal
        const sf::Color Deepest  (7, 9, 19);     // near black indigo
        const sf::Color NeonPink (247, 54, 152);
        const sf::Color NeonPink2(243, 55, 241);
        const sf::Color NeonCyan (48, 144, 199);
        const sf::Color NeonCyan2(0, 229, 255);
        const sf::Color NeonGold (233, 150, 104);
        const sf::Color TextMain (246, 224, 252);
        const sf::Color TextSub  (190, 198, 222);
    }

    namespace {

        sf::Texture globalBgTex;
        bool isGlobalBgLoaded = false;
        bool hasTriedLoadingBg = false;

        struct Particle {
            float x, y, radius, speedX, speedY, phase;
            sf::Color color;
        };

        std::vector<Particle> particles;
        bool particlesInit = false;

        void initParticles(float w, float h) {
            if (particlesInit) return;
            std::srand(1337);
            for (int i = 0; i < 60; ++i) {
                Particle p;
                p.x = (float)(std::rand() % (int)w);
                p.y = (float)(std::rand() % (int)h);
                p.radius = 1.0f + (float)(std::rand() % 30) / 20.f;
                p.speedX = ((float)(std::rand() % 200) / 100.f - 1.f) * 20.f;
                p.speedY = ((float)(std::rand() % 200) / 100.f - 1.f) * 20.f;
                p.phase = (float)(std::rand() % 628) / 100.f;
                int c = std::rand() % 4;
                if (c == 0) p.color = UIPalette::NeonPink;
                else if (c == 1) p.color = UIPalette::NeonCyan2;
                else if (c == 2) p.color = UIPalette::NeonCyan;
                else p.color = UIPalette::NeonGold;
                particles.push_back(p);
            }
            particlesInit = true;
        }

        void drawAnimatedNeonBackground(sf::RenderWindow& window, float anim, bool moving) {
            sf::Vector2u ws = window.getSize();
            float w = (float)ws.x, h = (float)ws.y;

            // 1. Translucent base so the photo backdrop (background.png) shows through,
            //    but a darkened neon tint keeps UI panels readable when no photo is present.
            sf::RectangleShape base(sf::Vector2f(w, h));
            base.setFillColor(sf::Color(7, 9, 19, isGlobalBgLoaded ? 120 : 235));
            window.draw(base);

            // 2. Radial-ish glow blobs for a city-light feel
            auto drawGlow = [&](float gx, float gy, float gr, sf::Color col) {
                sf::CircleShape c(gr);
                c.setPosition(sf::Vector2f(gx - gr, gy - gr));
                c.setFillColor(sf::Color(col.r, col.g, col.b, 60));
                window.draw(c);
                sf::CircleShape c2(gr * 0.5f);
                c2.setPosition(sf::Vector2f(gx - gr * 0.5f, gy - gr * 0.5f));
                c2.setFillColor(sf::Color(col.r, col.g, col.b, 90));
                window.draw(c2);
            };
            // Static city neon blobs (from image mood)
            drawGlow(w * 0.12f, h * 0.2f, w * 0.10f, UIPalette::NeonCyan);
            drawGlow(w * 0.85f, h * 0.15f, w * 0.12f, UIPalette::NeonPink);
            drawGlow(w * 0.9f, h * 0.8f, w * 0.14f, UIPalette::NeonGold);
            drawGlow(w * 0.08f, h * 0.9f, w * 0.11f, UIPalette::NeonPink2);

            // 3. Animated horizontal light streaks (city light trails)
            for (int i = 0; i < 6 && moving; ++i) {
                float y = h * (0.15f + i * 0.14f);
                float xoff = anim * (40.f + i * 15.f);
                float len = w * 0.18f;
                sf::RectangleShape streak(sf::Vector2f(len, 1.5f));
                streak.setFillColor(sf::Color(190, 200, 230, 70));
                streak.setPosition(sf::Vector2f(fmod(xoff, w + len) - len, y));
                window.draw(streak);
            }

            // 4. Floating neon particles
            initParticles(w, h);
            for (auto& p : particles) {
                if (moving) {
                    p.x += p.speedX * 0.016f;
                    p.y += p.speedY * 0.016f;
                    if (p.x < 0) p.x = w;
                    if (p.x > w) p.x = 0;
                    if (p.y < 0) p.y = h;
                    if (p.y > h) p.y = 0;
                }
                float tw = 0.5f + 0.5f * std::sin(p.phase + anim * 2.0f);
                float alpha = 90.f + 120.f * tw;
                sf::CircleShape dot(p.radius);
                dot.setFillColor(sf::Color(p.color.r, p.color.g, p.color.b, (std::uint8_t)alpha));
                dot.setPosition(sf::Vector2f(p.x, p.y));
                window.draw(dot);
            }

            // 5. Darkening vignette at edges for depth
            sf::RectangleShape vign(sf::Vector2f(w, h));
            vign.setFillColor(sf::Color(3, 1, 6, 110));
            window.draw(vign);
        }

        void drawBackground(sf::RenderWindow& window, float anim = 0.f) {
            // Use the actual background.png as the photo backdrop, then layer neon on top
            if (!hasTriedLoadingBg) {
                if (globalBgTex.loadFromFile("background.png")) {
                    globalBgTex.setSmooth(true);
                    isGlobalBgLoaded = true;
                }
                hasTriedLoadingBg = true;
            }

            if (isGlobalBgLoaded) {
                sf::Sprite bgSprite(globalBgTex);
                sf::Vector2u texSize = globalBgTex.getSize();
                sf::Vector2u winSize = window.getSize();
                if (texSize.x > 0 && texSize.y > 0) {
                    bgSprite.setScale(sf::Vector2f((float)winSize.x / texSize.x, (float)winSize.y / texSize.y));
                }
                window.draw(bgSprite);

                // Slight dark overlay so UI pops over the photo
                sf::RectangleShape overlay(sf::Vector2f((float)winSize.x, (float)winSize.y));
                overlay.setFillColor(sf::Color(7, 9, 19, 70));
                window.draw(overlay);
            } else {
                drawAnimatedNeonBackground(window, anim, false);
            }
        }

        void drawNeonPanel(sf::RenderWindow& window, const sf::Vector2f& pos, const sf::Vector2f& size,
                           sf::Color glowColor) {
            // Glass body
            sf::RectangleShape body(size);
            body.setFillColor(sf::Color(10, 16, 28, 215));
            body.setPosition(pos);
            window.draw(body);

            // Neon border (double-layered glow)
            sf::RectangleShape border(size);
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineThickness(1.5f);
            border.setOutlineColor(sf::Color(glowColor.r, glowColor.g, glowColor.b, 160));
            border.setPosition(pos);
            window.draw(border);

            sf::RectangleShape outerGlow(size + sf::Vector2f(6, 6));
            outerGlow.setFillColor(sf::Color::Transparent);
            outerGlow.setOutlineThickness(1.f);
            outerGlow.setOutlineColor(sf::Color(glowColor.r, glowColor.g, glowColor.b, 50));
            outerGlow.setPosition(pos - sf::Vector2f(3, 3));
            window.draw(outerGlow);

            // Glowing top accent bar
            sf::RectangleShape accent(sf::Vector2f(size.x, 3.f));
            accent.setFillColor(glowColor);
            accent.setPosition(sf::Vector2f(pos.x, pos.y));
            window.draw(accent);
        }

        void drawNeonInput(sf::RenderWindow& window, const sf::Vector2f& pos, const sf::Vector2f& size,
                           bool focused, const std::string& label, const sf::Font& font) {
            sf::Text lbl(font, label, 13);
            lbl.setFillColor(focused ? UIPalette::NeonCyan2 : sf::Color(160, 175, 205));
            lbl.setStyle(sf::Text::Bold);
            lbl.setPosition(sf::Vector2f(pos.x, pos.y - 21.f));
            window.draw(lbl);

            sf::Color body = focused ? sf::Color(18, 28, 48, 225) : sf::Color(12, 20, 36, 210);
            sf::Color border = focused ? UIPalette::NeonCyan2 : sf::Color(90, 110, 150);

            sf::RectangleShape box(size);
            box.setFillColor(body);
            box.setPosition(pos);
            window.draw(box);

            sf::RectangleShape outline(size + sf::Vector2f(2, 2));
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineThickness(focused ? 2.f : 1.f);
            outline.setOutlineColor(sf::Color(border.r, border.g, border.b, 200));
            outline.setPosition(pos - sf::Vector2f(1, 1));
            window.draw(outline);
        }

        void drawNeonButton(sf::RenderWindow& window, const sf::Vector2f& pos, const sf::Vector2f& size,
                            const std::string& text, const sf::Font& font, sf::Color base, float anim = 0.f) {
            float pulse = 0.5f + 0.5f * std::sin(anim * 3.0f);
            sf::Color baseBright(
                (std::uint8_t)(base.r + (std::uint8_t)(25 * pulse)),
                (std::uint8_t)(base.g + (std::uint8_t)(25 * pulse)),
                (std::uint8_t)(base.b + (std::uint8_t)(25 * pulse)));

            // Outer glow
            sf::RectangleShape halo(size + sf::Vector2f(8, 8));
            halo.setFillColor(sf::Color::Transparent);
            halo.setOutlineThickness(2.f);
            halo.setOutlineColor(sf::Color(base.r, base.g, base.b, (std::uint8_t)(60 + 60 * pulse)));
            halo.setPosition(pos - sf::Vector2f(4, 4));
            window.draw(halo);

            sf::RectangleShape btn(size);
            btn.setFillColor(baseBright);
            btn.setPosition(pos);
            window.draw(btn);

            sf::RectangleShape outline(size);
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineThickness(1.5f);
            outline.setOutlineColor(sf::Color(255, 255, 255, 90));
            outline.setPosition(pos);
            window.draw(outline);

            sf::Text lbl(font, text, 16);
            lbl.setStyle(sf::Text::Bold);
            lbl.setFillColor(sf::Color::White);
            sf::FloatRect tb = lbl.getLocalBounds();
            lbl.setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f));
            lbl.setPosition(sf::Vector2f(pos.x + size.x / 2.f, pos.y + size.y / 2.f));
            window.draw(lbl);
        }

        void drawNeonCard(sf::RenderWindow& window, const sf::Vector2f& pos, const sf::Vector2f& size,
                          sf::Color accent) {
            sf::RectangleShape body(size);
            body.setFillColor(sf::Color(12, 20, 34, 215));
            body.setPosition(pos);
            window.draw(body);

            sf::RectangleShape outline(size);
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineThickness(1.5f);
            outline.setOutlineColor(sf::Color(accent.r, accent.g, accent.b, 140));
            outline.setPosition(pos);
            window.draw(outline);

            sf::RectangleShape accentBar(sf::Vector2f(size.x, 3.f));
            accentBar.setFillColor(accent);
            accentBar.setPosition(sf::Vector2f(pos.x, pos.y));
            window.draw(accentBar);
        }
    }

    // ===================== LOGIN SCREEN =====================

    ScreenState* LoginScreen::handleInput(const sf::Event& event) {
        if (const auto* te = event.getIf<sf::Event::TextEntered>()) {
            if (te->unicode == '\b') {
                if (typingUname && !uname.empty()) uname.pop_back();
                else if (!typingUname && !pass.empty()) pass.pop_back();
                SoundManager::play(SFX::Click);
            } else if (te->unicode == '\t') {
                typingUname = !typingUname;
                SoundManager::play(SFX::Click);
            } else if (te->unicode >= 32 && te->unicode < 128) {
                SoundManager::play(SFX::Click);
                if (typingUname) uname += static_cast<char>(te->unicode);
                else pass += static_cast<char>(te->unicode);
            }
        }

        if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Enter) {
                if (uname.empty() || pass.empty()) {
                    SoundManager::play(SFX::Error);
                    statusMsg = "Please enter both username and password.";
                    statusTimer = 3.0f;
                    return this;
                }
                if (!AuthManager::loginUser(uname, pass)) {
                    if (AuthManager::registerUser(uname, pass)) {
                        AuthManager::loginUser(uname, pass);
                        SoundManager::play(SFX::Success);
                        statusMsg = "New account created! Welcome, " + uname + "!";
                        statusTimer = 2.0f;
                    } else {
                        SoundManager::play(SFX::Error);
                        statusMsg = "Invalid username or password.";
                        statusTimer = 3.0f;
                        return this;
                    }
                } else {
                    SoundManager::play(SFX::Success);
                }
                return new DashboardScreen();
            }
            if (kp->code == sf::Keyboard::Key::Tab) {
                SoundManager::play(SFX::Click);
            }
        }
        return this;
    }

    void LoginScreen::render(sf::RenderWindow& window, const sf::Font& font) {
        animTimer += 1.f / 60.f;
        cursorTimer += 1.f / 60.f;
        if (statusTimer > 0.f) statusTimer -= 1.f / 60.f;

        sf::Vector2u ws = window.getSize();
        float cx = ws.x / 2.f, cy = ws.y / 2.f;

        drawBackground(window, 0.f);
        drawAnimatedNeonBackground(window, animTimer, true);

        // Center neon panel
        sf::Vector2f panelSize(540.f, 500.f);
        drawNeonPanel(window, sf::Vector2f(cx - 270.f, cy - 250.f), panelSize, UIPalette::NeonCyan);

        // Title with neon glow
        sf::Text title(font, "KUET RICKSHAW RUSH", 36);
        title.setStyle(sf::Text::Bold);
        title.setFillColor(UIPalette::TextMain);
        float glowPulse = 0.6f + 0.4f * std::sin(animTimer * 2.5f);
        title.setOutlineThickness(2.f);
        title.setOutlineColor(sf::Color(
            (std::uint8_t)(UIPalette::NeonPink.r),
            (std::uint8_t)(UIPalette::NeonPink.g * 0.5f + glowPulse * 100.f),
            (std::uint8_t)(UIPalette::NeonPink.b)));
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f));
        title.setPosition(sf::Vector2f(cx, cy - 220.f));
        window.draw(title);

        // Subtitle
        sf::Text sub(font, "DRIVER PORTAL  //  NEON ACCESS", 13);
        sub.setFillColor(UIPalette::NeonCyan2);
        sf::FloatRect sb = sub.getLocalBounds();
        sub.setOrigin(sf::Vector2f(sb.position.x + sb.size.x / 2.f, sb.position.y + sb.size.y / 2.f));
        sub.setPosition(sf::Vector2f(cx, cy - 170.f));
        window.draw(sub);

        // Username field
        sf::Vector2f boxSize(400.f, 50.f);
        sf::Vector2f unameBoxPos(cx - 200.f, cy - 125.f);
        drawNeonInput(window, unameBoxPos, boxSize, typingUname, "USERNAME", font);

        bool cursorVisible = (int)(cursorTimer * 2) % 2 == 0;
        sf::Text unameText(font, uname + (typingUname && cursorVisible ? "|" : ""), 20);
        unameText.setFillColor(sf::Color::White);
        unameText.setPosition(sf::Vector2f(unameBoxPos.x + 14.f, unameBoxPos.y + 12.f));
        window.draw(unameText);

        // Password field
        sf::Vector2f passBoxPos(cx - 200.f, cy - 35.f);
        drawNeonInput(window, passBoxPos, boxSize, !typingUname, "PASSWORD", font);

        sf::Text passText(font, string(pass.length(), '*') + (!typingUname && cursorVisible ? "|" : ""), 20);
        passText.setFillColor(sf::Color::White);
        passText.setPosition(sf::Vector2f(passBoxPos.x + 14.f, passBoxPos.y + 12.f));
        window.draw(passText);

        // Hint
        sf::Text hint(font, "Press TAB to switch field", 11);
        hint.setFillColor(UIPalette::TextSub);
        sf::FloatRect hb = hint.getLocalBounds();
        hint.setOrigin(sf::Vector2f(hb.position.x + hb.size.x / 2.f, hb.position.y + hb.size.y / 2.f));
        hint.setPosition(sf::Vector2f(cx, passBoxPos.y + 62.f));
        window.draw(hint);

        // Sign-in button (neon magenta)
        drawNeonButton(window, sf::Vector2f(cx - 200.f, passBoxPos.y + 85.f), sf::Vector2f(400.f, 54.f),
                       "SIGN IN  /  REGISTER", font, UIPalette::NeonPink, animTimer);

        // Status message
        if (statusTimer > 0.f) {
            sf::Text st(font, statusMsg, 14);
            st.setFillColor(sf::Color(255, 120, 160));
            sf::FloatRect stb = st.getLocalBounds();
            st.setOrigin(sf::Vector2f(stb.position.x + stb.size.x / 2.f, stb.position.y + stb.size.y / 2.f));
            st.setPosition(sf::Vector2f(cx, passBoxPos.y + 170.f));
            window.draw(st);
        }

        // Footer
        sf::Text footer(font, "KUET CAMPUS EDITION  v3.0  //  NEON", 11);
        footer.setFillColor(UIPalette::TextSub);
        sf::FloatRect fb = footer.getLocalBounds();
        footer.setOrigin(sf::Vector2f(fb.position.x + fb.size.x / 2.f, fb.position.y + fb.size.y / 2.f));
        footer.setPosition(sf::Vector2f(cx, cy + 230.f));
        window.draw(footer);
    }

    // ===================== DASHBOARD SCREEN =====================

    ScreenState* DashboardScreen::handleInput(const sf::Event& event) {
        if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
            int maxMission = AuthManager::getCurrentUser().getUnlockedMission();
            int pressed = -1;

            if (kp->code == sf::Keyboard::Key::Num1) pressed = 0;
            else if (kp->code == sf::Keyboard::Key::Num2) pressed = 1;
            else if (kp->code == sf::Keyboard::Key::Num3) pressed = 2;
            else if (kp->code == sf::Keyboard::Key::Num4) pressed = 3;
            else if (kp->code == sf::Keyboard::Key::Num5) pressed = 4;

            if (pressed >= 0 && pressed < maxMission) {
                SoundManager::play(SFX::Click);
                return new PlayingScreen(pressed);
            }

            // Open the garage / upgrade menu
            if (kp->code == sf::Keyboard::Key::U || kp->code == sf::Keyboard::Key::G) {
                SoundManager::play(SFX::Click);
                return new UpgradeScreen();
            }

            // Open the custom mission builder
            if (kp->code == sf::Keyboard::Key::C) {
                SoundManager::play(SFX::Click);
                return new CustomMissionScreen();
            }
        }
        return this;
    }

    void DashboardScreen::render(sf::RenderWindow& window, const sf::Font& font) {
        animTimer += 1.f / 60.f;
        sf::Vector2u ws = window.getSize();
        float cx = ws.x / 2.f;

        drawBackground(window, 0.f);
        drawAnimatedNeonBackground(window, animTimer, true);

        // Header neon panel
        drawNeonPanel(window, sf::Vector2f(cx - 440.f, 30.f), sf::Vector2f(880.f, 120.f), UIPalette::NeonGold);

        sf::Text title(font, "DRIVER DASHBOARD", 34);
        title.setStyle(sf::Text::Bold);
        title.setFillColor(UIPalette::TextMain);
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, 0));
        title.setPosition(sf::Vector2f(cx, 45.f));
        window.draw(title);

        std::string unameStr = AuthManager::getCurrentUser().getUsername();
        sf::Text welc(font, "WELCOME BACK, " + unameStr, 18);
        welc.setFillColor(UIPalette::NeonGold);
        welc.setStyle(sf::Text::Bold);
        sf::FloatRect wb = welc.getLocalBounds();
        welc.setOrigin(sf::Vector2f(wb.position.x + wb.size.x / 2.f, 0));
        welc.setPosition(sf::Vector2f(cx, 112.f));
        window.draw(welc);

        int earnings = AuthManager::getCurrentUser().getTotalEarnings();
        int unlocked = AuthManager::getCurrentUser().getUnlockedMission();
        int highScore = AuthManager::getCurrentUser().getHighestScore();

        // Stat cards row
        const float cardW = 270.f, cardH = 150.f, gap = 24.f;
        float startX = cx - (cardW * 3 + gap * 2) / 2.f;
        float cardY = 185.f;

        struct StatCard { std::string label; std::string value; std::string unit; sf::Color color; };
        StatCard cards[3] = {
            {"TOTAL BALANCE", to_string(earnings), "TK", UIPalette::NeonGold},
            {"MISSIONS UNLOCKED", to_string(unlocked) + " / 5", "", UIPalette::NeonCyan2},
            {"HIGHEST SCORE", to_string(highScore), "XP", UIPalette::NeonPink2}
        };

        for (int i = 0; i < 3; ++i) {
            float x = startX + i * (cardW + gap);
            drawNeonCard(window, sf::Vector2f(x, cardY), sf::Vector2f(cardW, cardH), cards[i].color);

            sf::Text clbl(font, cards[i].label, 13);
            clbl.setFillColor(UIPalette::TextSub);
            sf::FloatRect clb = clbl.getLocalBounds();
            clbl.setOrigin(sf::Vector2f(clb.position.x + clb.size.x / 2.f, 0));
            clbl.setPosition(sf::Vector2f(x + cardW / 2.f, cardY + 22.f));
            window.draw(clbl);

            sf::Text cval(font, cards[i].value, 34);
            cval.setStyle(sf::Text::Bold);
            cval.setFillColor(cards[i].color);
            sf::FloatRect cvb = cval.getLocalBounds();
            cval.setOrigin(sf::Vector2f(cvb.position.x + cvb.size.x / 2.f, 0));
            cval.setPosition(sf::Vector2f(x + cardW / 2.f, cardY + 62.f));
            window.draw(cval);

            if (!cards[i].unit.empty()) {
                sf::Text cunit(font, cards[i].unit, 15);
                cunit.setFillColor(sf::Color(200, 210, 230));
                sf::FloatRect cub = cunit.getLocalBounds();
                cunit.setOrigin(sf::Vector2f(cub.position.x + cub.size.x / 2.f, 0));
                cunit.setPosition(sf::Vector2f(x + cardW / 2.f, cardY + 108.f));
                window.draw(cunit);
            }
        }

        // Mission select panel
        float mY = cardY + cardH + 30.f;
        drawNeonPanel(window, sf::Vector2f(cx - 440.f, mY), sf::Vector2f(880.f, 300.f), UIPalette::NeonCyan);

        sf::Text mh(font, "SELECT MISSION", 24);
        mh.setStyle(sf::Text::Bold);
        mh.setFillColor(UIPalette::NeonCyan2);
        sf::FloatRect mhb = mh.getLocalBounds();
        mh.setOrigin(sf::Vector2f(mhb.position.x + mhb.size.x / 2.f, 0));
        mh.setPosition(sf::Vector2f(cx, mY + 18.f));
        window.draw(mh);

        struct MissionInfo { std::string dest; std::string fare; };
        MissionInfo missions[5] = {
            {"Main Gate -> CSE Building", "Fare: 50 TK"},
            {"CSE Building -> KUET Library", "Fare: 75 TK"},
            {"KUET Library -> Shaheed Minar", "Fare: 60 TK"},
            {"Shaheed Minar -> Amar Ekushey Hall", "Fare: 80 TK"},
            {"Amar Ekushey Hall -> Main Gate", "Fare: 150 TK"}
        };

        const float btnW = 160.f, btnH = 52.f, btnGap = 14.f;
        float bStartX = cx - (btnW * 5 + btnGap * 4) / 2.f;

        for (int i = 0; i < 5; ++i) {
            float x = bStartX + i * (btnW + btnGap);
            bool locked = (i >= unlocked);
            // Gradient of neon colors for unlocked buttons
            static const sf::Color grad[5] = {
                UIPalette::NeonCyan2, UIPalette::NeonCyan, UIPalette::NeonGold,
                UIPalette::NeonPink, UIPalette::NeonPink2
            };
            sf::Color btnBase = locked ? sf::Color(60, 66, 82) : grad[i];

            drawNeonButton(window, sf::Vector2f(x, mY + 60.f), sf::Vector2f(btnW, btnH),
                           "MISSION " + to_string(i + 1), font, btnBase, animTimer);

            sf::Text mnum(font, missions[i].dest, 10);
            mnum.setFillColor(locked ? sf::Color(120, 130, 150) : UIPalette::TextSub);
            sf::FloatRect mnb = mnum.getLocalBounds();
            mnum.setOrigin(sf::Vector2f(mnb.position.x + mnb.size.x / 2.f, 0));
            mnum.setPosition(sf::Vector2f(x + btnW / 2.f, mY + 120.f));
            window.draw(mnum);

            sf::Text mfare(font, missions[i].fare, 10);
            mfare.setFillColor(UIPalette::NeonGold);
            sf::FloatRect mfb = mfare.getLocalBounds();
            mfare.setOrigin(sf::Vector2f(mfb.position.x + mfb.size.x / 2.f, 0));
            mfare.setPosition(sf::Vector2f(x + btnW / 2.f, mY + 140.f));
            window.draw(mfare);

            if (locked) {
                sf::Text lock(font, "LOCKED", 10);
                lock.setStyle(sf::Text::Bold);
                lock.setFillColor(sf::Color(255, 90, 120));
                sf::FloatRect lb = lock.getLocalBounds();
                lock.setOrigin(sf::Vector2f(lb.position.x + lb.size.x / 2.f, 0));
                lock.setPosition(sf::Vector2f(x + btnW / 2.f, mY + 158.f));
                window.draw(lock);
            }
        }

        sf::Text in(font, "Press a number key (1-5) to start a mission", 15);
        in.setFillColor(UIPalette::TextSub);
        sf::FloatRect ib = in.getLocalBounds();
        in.setOrigin(sf::Vector2f(ib.position.x + ib.size.x / 2.f, 0));
        in.setPosition(sf::Vector2f(cx, mY + 225.f));
        window.draw(in);

        // Prominent GARAGE button (upgrade shop)
        float gBtnY = mY + 310.f;
        drawNeonButton(window, sf::Vector2f(cx - 240.f, gBtnY), sf::Vector2f(480.f, 58.f),
                       "GARAGE / UPGRADES   [ U ]", font, UIPalette::NeonPink, animTimer);
        sf::Text gSub(font, "Buy speed, fuel tank, armor, turbo & paint with your TK", 12);
        gSub.setFillColor(UIPalette::NeonCyan2);
        sf::FloatRect gb = gSub.getLocalBounds();
        gSub.setOrigin(sf::Vector2f(gb.position.x + gb.size.x / 2.f, 0));
        gSub.setPosition(sf::Vector2f(cx, gBtnY + 62.f));
        window.draw(gSub);
    }

    // ===================== PLAYING SCREEN =====================

    ScreenState* PlayingScreen::handleInput(const sf::Event& event) {
        return this;
    }

    void PlayingScreen::render(sf::RenderWindow& window, const sf::Font& font) {
        // In-game map rendering happens in main.cpp via dynamic_cast.
        // This method exists to satisfy the interface.
    }

    // ===================== CUSTOM MISSION BUILDER =====================

    CustomMissionScreen::CustomMissionScreen() {
        // Default fare/time
        fare = 100;
        timeLimit = 60.0f;
        pickupSet = false;
        destSet = false;
    }

    ScreenState* CustomMissionScreen::handleInput(const sf::Event& event) {
        if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
            float step = 15.0f;

            // Move the crosshair
            if (kp->code == sf::Keyboard::Key::W || kp->code == sf::Keyboard::Key::Up)    cursorY -= step;
            if (kp->code == sf::Keyboard::Key::S || kp->code == sf::Keyboard::Key::Down)  cursorY += step;
            if (kp->code == sf::Keyboard::Key::A || kp->code == sf::Keyboard::Key::Left)  cursorX -= step;
            if (kp->code == sf::Keyboard::Key::D || kp->code == sf::Keyboard::Key::Right) cursorX += step;

            // Clamp to the campus map
            cursorX = std::max(0.f, std::min(2000.f, cursorX));
            cursorY = std::max(0.f, std::min(1500.f, cursorY));

            // Set pickup / destination (snapped to nearest road)
            std::vector<CampusRoad> roads = buildCampusRoads();
            auto snap = [&](float& x, float& y) {
                float bestD2 = 1e18f;
                float bx = x, by = y;
                for (const auto& r : roads) {
                    float laneX = r.x + r.width * 0.5f;
                    float laneY = r.y + r.height * 0.5f;
                    float tx, ty;
                    if (r.isVertical) {
                        tx = laneX;
                        ty = std::max(r.y, std::min(y, r.y + r.height));
                    } else {
                        ty = laneY;
                        tx = std::max(r.x, std::min(x, r.x + r.width));
                    }
                    float dx = tx - x, dy = ty - y;
                    float d2 = dx*dx + dy*dy;
                    if (d2 < bestD2) { bestD2 = d2; bx = tx; by = ty; }
                }
                x = bx;
                y = by;
            };

            if (kp->code == sf::Keyboard::Key::Space) {
                SoundManager::play(SFX::Click);
                float sx = cursorX, sy = cursorY;
                snap(sx, sy);
                if (!pickupSet) {
                    pickupX = sx; pickupY = sy;
                    pickupSet = true;
                    statusMsg = "Pickup set! Now move to the DESTINATION and press SPACE.";
                    statusTimer = 3.0f;
                } else if (!destSet) {
                    destX = sx; destY = sy;
                    destSet = true;
                    statusMsg = "Destination set! Adjust fare / time and press ENTER to ride.";
                    statusTimer = 3.0f;
                }
            }

            // Fare / time tuning
            if (kp->code == sf::Keyboard::Key::Equal || kp->code == sf::Keyboard::Key::Add)      fare += 20;
            if (kp->code == sf::Keyboard::Key::Subtract || kp->code == sf::Keyboard::Key::Hyphen) fare -= 20;
            if (kp->code == sf::Keyboard::Key::RBracket) timeLimit += 5.0f;
            if (kp->code == sf::Keyboard::Key::LBracket) timeLimit -= 5.0f;
            fare = std::max(20, std::min(1000, fare));
            timeLimit = std::max(10.f, std::min(300.f, timeLimit));

            // Start the custom mission
            if (kp->code == sf::Keyboard::Key::Enter && pickupSet && destSet) {
                SoundManager::play(SFX::Click);
                return new PlayingScreen(pickupX, pickupY, destX, destY, fare, timeLimit);
            }

            // Go back to dashboard
            if (kp->code == sf::Keyboard::Key::Escape || kp->code == sf::Keyboard::Key::C) {
                SoundManager::play(SFX::Click);
                return new DashboardScreen();
            }
        }
        return this;
    }

    void CustomMissionScreen::render(sf::RenderWindow& window, const sf::Font& font) {
        animTimer += 1.f / 60.f;
        if (statusTimer > 0.f) statusTimer -= 1.f / 60.f;

        sf::Vector2u ws = window.getSize();
        float cx = ws.x / 2.f;

        drawBackground(window, 0.f);
        drawAnimatedNeonBackground(window, animTimer, false);

        // Header panel
        drawNeonPanel(window, sf::Vector2f(cx - 470.f, 15.f), sf::Vector2f(940.f, 70.f), UIPalette::NeonCyan);
        sf::Text title(font, "CUSTOM MISSION BUILDER", 28);
        title.setStyle(sf::Text::Bold);
        title.setFillColor(UIPalette::TextMain);
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, 0));
        title.setPosition(sf::Vector2f(cx, 28.f));
        window.draw(title);

        // Campus mini-map
        sf::RectangleShape mapBg(sf::Vector2f(1200.f, 850.f));
        mapBg.setFillColor(sf::Color(28, 34, 48, 240));
        mapBg.setOutlineThickness(2.f);
        mapBg.setOutlineColor(sf::Color(120, 140, 175));
        mapBg.setPosition(sf::Vector2f(cx - 600.f, 105.f));
        window.draw(mapBg);

        float mw = 1200.f, mh = 850.f;
        float scaleW = mw / 2000.f;
        float scaleH = mh / 1500.f;
        float ox = cx - 600.f, oy = 105.f;
        auto toScreen = [&](float wx, float wy) {
            return sf::Vector2f(ox + wx * scaleW, oy + wy * scaleH);
        };

        // Water / lake backdrop
        sf::RectangleShape water(sf::Vector2f(mw, mh));
        water.setFillColor(sf::Color(30, 60, 90, 90));
        window.draw(water);

        // Buildings (colored rectangles)
        auto cbd = buildCampusBuildings();
        for (const auto& b : cbd) {
            sf::Vector2f p = toScreen(b.x, b.y);
            sf::Vector2f s(b.width * scaleW, b.height * scaleH);
            sf::RectangleShape r(s);
            r.setFillColor(b.color);
            r.setPosition(p);
            window.draw(r);
        }

        // Roads (light gray bands with a center line)
        auto crd = buildCampusRoads();
        for (const auto& rd : crd) {
            sf::Vector2f p = toScreen(rd.x, rd.y);
            sf::Vector2f s(rd.width * scaleW, rd.height * scaleH);
            sf::RectangleShape r(s);
            r.setFillColor(sf::Color(150, 155, 165));
            r.setOutlineThickness(1.f);
            r.setOutlineColor(sf::Color(200, 205, 215));
            r.setPosition(p);
            window.draw(r);
        }

        // Crosshair
        sf::Vector2f ch = toScreen(cursorX, cursorY);
        sf::CircleShape ring(12.f);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.5f);
        ring.setOutlineColor(sf::Color(80, 240, 255));
        ring.setOrigin(sf::Vector2f(12.f, 12.f));
        ring.setPosition(ch);
        window.draw(ring);
        sf::CircleShape dot(3.f);
        dot.setFillColor(sf::Color(80, 240, 255));
        dot.setOrigin(sf::Vector2f(3.f, 3.f));
        dot.setPosition(ch);
        window.draw(dot);

        // Pickup marker
        if (pickupSet) {
            sf::Vector2f pp = toScreen(pickupX, pickupY);
            sf::CircleShape pm(11.f);
            pm.setFillColor(sf::Color(60, 220, 90, 200));
            pm.setOutlineThickness(2.f);
            pm.setOutlineColor(sf::Color::White);
            pm.setOrigin(sf::Vector2f(11.f, 11.f));
            pm.setPosition(pp);
            window.draw(pm);
            sf::Text pt(font, "PICKUP", 13);
            pt.setFillColor(sf::Color(60, 220, 90));
            pt.setPosition(pp + sf::Vector2f(16.f, -6.f));
            window.draw(pt);
        }

        // Destination marker
        if (destSet) {
            sf::Vector2f dp = toScreen(destX, destY);
            sf::CircleShape dm(11.f);
            dm.setFillColor(sf::Color(255, 200, 0, 200));
            dm.setOutlineThickness(2.f);
            dm.setOutlineColor(sf::Color::White);
            dm.setOrigin(sf::Vector2f(11.f, 11.f));
            dm.setPosition(dp);
            window.draw(dm);
            sf::Text dt(font, "DEST", 13);
            dt.setFillColor(sf::Color(255, 200, 0));
            dt.setPosition(dp + sf::Vector2f(16.f, -6.f));
            window.draw(dt);
        }

        // Right-hand control panel
        float panelX = cx + 640.f;
        drawNeonPanel(window, sf::Vector2f(panelX, 105.f), sf::Vector2f(300.f, 480.f), UIPalette::NeonGold);

        sf::Text stepTxt(font, "STEP 1:  Pick a PICKUP point", 15);
        stepTxt.setFillColor(pickupSet ? sf::Color(60, 220, 90) : sf::Color(235, 240, 250));
        stepTxt.setPosition(sf::Vector2f(panelX + 16.f, 130.f));
        window.draw(stepTxt);

        sf::Text step2Txt(font, "STEP 2:  Pick a DESTINATION", 15);
        step2Txt.setFillColor(destSet ? sf::Color(255, 200, 0) : sf::Color(235, 240, 250));
        step2Txt.setPosition(sf::Vector2f(panelX + 16.f, 180.f));
        window.draw(step2Txt);

        sf::Text tipTxt(font, "Move  WASD / Arrows\n"
                              "SPACE  set point\n"
                              "[ ]  time   (-/+)  fare\n"
                              "ENTER  start ride   ESC  back",
                    14);
        tipTxt.setFillColor(sf::Color(190, 205, 225));
        tipTxt.setPosition(sf::Vector2f(panelX + 16.f, 240.f));
        window.draw(tipTxt);

        sf::Text fareTxt(font, "FARE:  " + std::to_string(fare) + " TK", 22);
        fareTxt.setStyle(sf::Text::Bold);
        fareTxt.setFillColor(UIPalette::NeonPink);
        sf::FloatRect fbt = fareTxt.getLocalBounds();
        fareTxt.setOrigin(sf::Vector2f(fbt.position.x + fbt.size.x / 2.f, 0));
        fareTxt.setPosition(sf::Vector2f(panelX + 150.f, 400.f));
        window.draw(fareTxt);

        sf::Text timeTxt(font, "TIME:  " + std::to_string((int)timeLimit) + " s", 22);
        timeTxt.setStyle(sf::Text::Bold);
        timeTxt.setFillColor(UIPalette::NeonCyan2);
        sf::FloatRect tbt = timeTxt.getLocalBounds();
        timeTxt.setOrigin(sf::Vector2f(tbt.position.x + tbt.size.x / 2.f, 0));
        timeTxt.setPosition(sf::Vector2f(panelX + 150.f, 440.f));
        window.draw(timeTxt);

        if (pickupSet && destSet)
            drawNeonPanel(window, sf::Vector2f(panelX, 530.f), sf::Vector2f(300.f, 60.f), UIPalette::NeonPink);

        // Status / footer
        if (statusTimer > 0.f) {
            sf::Text st(font, statusMsg, 15);
            st.setFillColor(UIPalette::NeonCyan2);
            st.setPosition(sf::Vector2f(cx - 560.f, ws.y - 60.f));
            window.draw(st);
        }
        sf::Text foot(font, "ESC / C  to return to Dashboard", 13);
        foot.setFillColor(UIPalette::TextSub);
        sf::FloatRect fb = foot.getLocalBounds();
        foot.setOrigin(sf::Vector2f(fb.position.x + fb.size.x / 2.f, 0));
        foot.setPosition(sf::Vector2f(cx, ws.y - 30.f));
        window.draw(foot);
    }

    // ===================== UPGRADE SCREEN (Garage) =====================

    ScreenState* UpgradeScreen::handleInput(const sf::Event& event) {
        if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
            // Escape / Back returns to dashboard
            if (kp->code == sf::Keyboard::Key::Escape || kp->code == sf::Keyboard::Key::Backspace) {
                SoundManager::play(SFX::Click);
                return new DashboardScreen();
            }

            UserProfile& u = AuthManager::getCurrentUser();
            int earnings = u.getTotalEarnings();

            // Upgrade costs per level
            int speedCosts[3] = {300, 500, 800};
            int fuelCosts[3]  = {200, 350, 600};
            int armorCosts[3]= {250, 400, 650};
            constexpr int TURBO_COST = 1000;
            constexpr int PAINT_COST = 150;

            auto buy = [&](int cost, const std::string& what, auto apply) -> bool {
                if (cost < 0) {
                    SoundManager::play(SFX::Error);
                    statusMsg = "MAX LEVEL reached for " + what + "!";
                    statusTimer = 3.f;
                    return false;
                }
                if (earnings < cost) {
                    SoundManager::play(SFX::Error);
                    statusMsg = "Not enough balance! Need " + std::to_string(cost) + " TK for " + what + ".";
                    statusTimer = 3.5f;
                    return false;
                }
                u.spendEarnings(cost);
                apply();
                AuthManager::saveUpgrades();
                SoundManager::play(SFX::Success);
                statusMsg = "Purchased: " + what + "!  Balance: " + std::to_string(u.getTotalEarnings()) + " TK";
                statusTimer = 3.f;
                return true;
            };

            // 1: Speed upgrade
            if (kp->code == sf::Keyboard::Key::Num1) {
                selected = 1;
                int cur = u.getSpeedUpgrade();
                int cost = (cur < 3) ? speedCosts[cur] : -1;
                buy(cost, "Speed Engine", [&]{ u.setSpeedUpgrade(cur + 1); });
            }
            // 2: Fuel tank
            else if (kp->code == sf::Keyboard::Key::Num2) {
                selected = 2;
                int cur = u.getFuelUpgrade();
                int cost = (cur < 3) ? fuelCosts[cur] : -1;
                buy(cost, "Fuel Tank", [&]{ u.setFuelUpgrade(cur + 1); });
            }
            // 3: Armor
            else if (kp->code == sf::Keyboard::Key::Num3) {
                selected = 3;
                int cur = u.getArmorUpgrade();
                int cost = (cur < 3) ? armorCosts[cur] : -1;
                buy(cost, "Armor Plating", [&]{ u.setArmorUpgrade(cur + 1); });
            }
            // 4: Turbo unlock (one-time)
            else if (kp->code == sf::Keyboard::Key::Num4) {
                selected = 4;
                if (u.getTurboUpgrade() > 0) {
                    SoundManager::play(SFX::Error);
                    statusMsg = "Turbo is already unlocked!";
                    statusTimer = 3.f;
                } else {
                    buy(TURBO_COST, "Permanent Turbo", [&]{ u.setTurboUpgrade(1); });
                }
            }
            // 5-9: Paint schemes
            else if (kp->code == sf::Keyboard::Key::Num5 || kp->code == sf::Keyboard::Key::Num6 ||
                     kp->code == sf::Keyboard::Key::Num7 || kp->code == sf::Keyboard::Key::Num8 ||
                     kp->code == sf::Keyboard::Key::Num9) {
                int idx = kp->code == sf::Keyboard::Key::Num5 ? 0 :
                          kp->code == sf::Keyboard::Key::Num6 ? 1 :
                          kp->code == sf::Keyboard::Key::Num7 ? 2 :
                          kp->code == sf::Keyboard::Key::Num8 ? 3 : 4;
                selected = 5 + idx;
                std::string names[5] = {"Classic Yellow", "Crystal Blue", "Racing Red", "Royal Gold", "Night Shadow"};
                buy(PAINT_COST, "Paint: " + names[idx], [&]{ u.setPaintIndex(idx); });
            }
            // Arrow keys move selection
            else if (kp->code == sf::Keyboard::Key::Down) {
                selected = std::min(9, selected + 1);
                SoundManager::play(SFX::Click);
            }
            else if (kp->code == sf::Keyboard::Key::Up) {
                selected = std::max(1, selected - 1);
                SoundManager::play(SFX::Click);
            }
        }
        return this;
    }

    void UpgradeScreen::render(sf::RenderWindow& window, const sf::Font& font) {
        animTimer += 1.f / 60.f;
        if (statusTimer > 0.f) statusTimer -= 1.f / 60.f;

        sf::Vector2u ws = window.getSize();
        float cx = ws.x / 2.f;

        drawBackground(window, 0.f);
        drawAnimatedNeonBackground(window, animTimer, true);

        UserProfile& u = AuthManager::getCurrentUser();
        int earnings = u.getTotalEarnings();

        // Header
        drawNeonPanel(window, sf::Vector2f(cx - 460.f, 25.f), sf::Vector2f(920.f, 90.f), UIPalette::NeonGold);
        sf::Text title(font, "RICKSHAW GARAGE / UPGRADES", 32);
        title.setStyle(sf::Text::Bold);
        title.setFillColor(UIPalette::TextMain);
        sf::FloatRect tb = title.getLocalBounds();
        title.setOrigin(sf::Vector2f(tb.position.x + tb.size.x / 2.f, 0));
        title.setPosition(sf::Vector2f(cx, 42.f));
        window.draw(title);

        sf::Text bal(font, "BALANCE: " + std::to_string(earnings) + " TK", 20);
        bal.setFillColor(UIPalette::NeonGold);
        bal.setStyle(sf::Text::Bold);
        sf::FloatRect bb = bal.getLocalBounds();
        bal.setOrigin(sf::Vector2f(bb.position.x + bb.size.x / 2.f, 0));
        bal.setPosition(sf::Vector2f(cx, 92.f));
        window.draw(bal);

        // Upgrade option cards
        struct Opt {
            std::string key, name, desc, cost;
            int level; int max; sf::Color col; int id;
        };
        Opt opts[4] = {
            {"1", "Speed Engine",   "Higher top speed",        "Level: ", u.getSpeedUpgrade(), 3, UIPalette::NeonPink,  1},
            {"2", "Fuel Tank",      "Bigger fuel capacity",    "Level: ", u.getFuelUpgrade(),  3, UIPalette::NeonCyan2, 2},
            {"3", "Armor Plating",  "Reduces damage taken",    "Level: ", u.getArmorUpgrade(), 3, UIPalette::NeonGold,  3},
            {"4", "Permanent Turbo","Boost unlocked (1-time)", "Status: ", u.getTurboUpgrade(), 1, UIPalette::NeonPink2, 4}
        };

        const float cardW = 420.f, cardH = 120.f;
        float startY = 150.f;
        for (int i = 0; i < 4; ++i) {
            float y = startY + i * (cardH + 12.f);
            bool hover = (selected == opts[i].id);
            drawNeonCard(window, sf::Vector2f(cx - cardW / 2.f, y), sf::Vector2f(cardW, cardH),
                         hover ? opts[i].col : sf::Color(70, 80, 100));

            sf::Text knum(font, opts[i].key, 22);
            knum.setStyle(sf::Text::Bold);
            knum.setFillColor(hover ? opts[i].col : UIPalette::TextSub);
            knum.setPosition(sf::Vector2f(cx - cardW / 2.f + 18.f, y + 18.f));
            window.draw(knum);

            sf::Text nm(font, opts[i].name, 22);
            nm.setStyle(sf::Text::Bold);
            nm.setFillColor(opts[i].col);
            nm.setPosition(sf::Vector2f(cx - cardW / 2.f + 50.f, y + 14.f));
            window.draw(nm);

            sf::Text dc(font, opts[i].desc, 14);
            dc.setFillColor(UIPalette::TextSub);
            dc.setPosition(sf::Vector2f(cx - cardW / 2.f + 50.f, y + 45.f));
            window.draw(dc);

            // Show level pips / status + cost
            sf::Text costTxt(font, "", 14);
            if (opts[i].id == 4) {
                costTxt.setString(u.getTurboUpgrade() > 0 ? "UNLOCKED" : "Cost: 1000 TK (press 4)");
                costTxt.setFillColor(u.getTurboUpgrade() > 0 ? UIPalette::NeonCyan2 : UIPalette::NeonGold);
            } else {
                if (opts[i].level >= opts[i].max) {
                    costTxt.setString("MAX LEVEL");
                    costTxt.setFillColor(UIPalette::NeonCyan2);
                } else {
                    int cost = (opts[i].id == 1) ? (opts[i].level == 0 ? 300 : opts[i].level == 1 ? 500 : 800)
                              : (opts[i].id == 2) ? (opts[i].level == 0 ? 200 : opts[i].level == 1 ? 350 : 600)
                              : (opts[i].level == 0 ? 250 : opts[i].level == 1 ? 400 : 650);
                    costTxt.setString(std::to_string(opts[i].level) + " / " + std::to_string(opts[i].max) +
                                      "  |  Cost: " + std::to_string(cost) + " TK");
                    costTxt.setFillColor(hover ? UIPalette::NeonCyan2 : UIPalette::NeonGold);
                }
            }
            costTxt.setPosition(sf::Vector2f(cx - cardW / 2.f + 18.f, y + cardH - 30.f));
            window.draw(costTxt);
        }

        // Paint schemes row
        drawNeonPanel(window, sf::Vector2f(cx - 460.f, startY + 4 * (cardH + 12.f) + 6.f),
                      sf::Vector2f(920.f, 120.f), UIPalette::NeonCyan);
        sf::Text ph(font, "PAINT COLORS  (press 5-9, cost 150 TK)", 18);
        ph.setStyle(sf::Text::Bold);
        ph.setFillColor(UIPalette::NeonCyan2);
        ph.setPosition(sf::Vector2f(cx - 440.f, startY + 4 * (cardH + 12.f) + 16.f));
        window.draw(ph);

        sf::Color paintCols[5] = {
            sf::Color(255, 215, 0),   // classic yellow
            sf::Color(60, 160, 255),  // crystal blue
            sf::Color(235, 60, 60),   // racing red
            sf::Color(233, 150, 104), // royal gold
            sf::Color(70, 70, 90)     // night shadow
        };
        std::string paintNames[5] = {"Yellow", "Blue", "Red", "Gold", "Shadow"};
        float pY = startY + 4 * (cardH + 12.f) + 60.f;
        for (int i = 0; i < 5; ++i) {
            float px = cx - 420.f + i * 175.f;
            bool sel = (u.getPaintIndex() == i);
            sf::RectangleShape chip(sf::Vector2f(140.f, 44.f));
            chip.setFillColor(paintCols[i]);
            chip.setOutlineThickness(sel ? 3.f : 1.5f);
            chip.setOutlineColor(sel ? sf::Color::White : sf::Color(120, 130, 150));
            chip.setPosition(sf::Vector2f(px, pY));
            window.draw(chip);

            sf::Text cn(font, std::to_string(5 + i) + " " + paintNames[i], 13);
            cn.setStyle(sf::Text::Bold);
            cn.setFillColor(sf::Color::Black);
            sf::FloatRect cnb = cn.getLocalBounds();
            cn.setOrigin(sf::Vector2f(cnb.position.x + cnb.size.x / 2.f, cnb.position.y + cnb.size.y / 2.f));
            cn.setPosition(sf::Vector2f(px + 70.f, pY + 16.f));
            window.draw(cn);
        }

        // Footer / status
        if (statusTimer > 0.f) {
            sf::Text st(font, statusMsg, 15);
            st.setFillColor(UIPalette::NeonCyan2);
            sf::FloatRect stb = st.getLocalBounds();
            st.setOrigin(sf::Vector2f(stb.position.x + stb.size.x / 2.f, stb.position.y + stb.size.y / 2.f));
            st.setPosition(sf::Vector2f(cx, startY + 4 * (cardH + 12.f) + 160.f));
            window.draw(st);
        }

        sf::Text foot(font, "ESC / Backspace to return to Dashboard", 13);
        foot.setFillColor(UIPalette::TextSub);
        sf::FloatRect fb = foot.getLocalBounds();
        foot.setOrigin(sf::Vector2f(fb.position.x + fb.size.x / 2.f, 0));
        foot.setPosition(sf::Vector2f(cx, ws.y - 30.f));
        window.draw(foot);
    }
}
