#include "../include/Passenger.h"
#include <iostream>

using namespace KUET;

Passenger::Passenger(float sx, float sy, std::string name, std::string dest, int amount)
    : GameObject(sx, sy), passengerName(name), destination(dest), fare(amount), 
      isPickedUp(false), missionId(-1), pickupPrompt(false) {

    // Collision circle (invisible-ish, used for pickup bounds)
    shape.setRadius(14.f);
    shape.setFillColor(sf::Color(0, 0, 0, 0));
    shape.setPosition(sf::Vector2f(x, y));

    // Shadow
    shadow.setRadius(15.f);
    shadow.setScale(sf::Vector2f(1.3f, 0.5f));
    shadow.setFillColor(sf::Color(0, 0, 0, 50));

    // Head
    head.setRadius(7.f);
    head.setFillColor(sf::Color(235, 205, 175)); // skin
    head.setOutlineThickness(1);
    head.setOutlineColor(sf::Color(0, 0, 0, 60));

    // Torso (shirt)
    torso.setSize(sf::Vector2f(12.f, 16.f));
    torso.setFillColor(sf::Color(200, 60, 70));  // red shirt

    // Arms (different shades for sleeve effect)
    armL.setSize(sf::Vector2f(4.f, 12.f));
    armL.setFillColor(sf::Color(235, 205, 175));
    armR.setSize(sf::Vector2f(4.f, 12.f));
    armR.setFillColor(sf::Color(235, 205, 175));

    // Legs (pants)
    legL.setSize(sf::Vector2f(5.f, 14.f));
    legL.setFillColor(sf::Color(50, 60, 110));   // blue pants
    legR.setSize(sf::Vector2f(5.f, 14.f));
    legR.setFillColor(sf::Color(50, 60, 110));
}

void Passenger::update(float dt) {
    if (!isPickedUp) {
        animTimer += dt;
    }
}

void Passenger::update(Rickshaw* player) {
    if (!isPickedUp) {
        // Distance-based pickup: much friendlier than exact circle overlap
        float dx = player->getX() - x;
        float dy = player->getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        constexpr float PICKUP_RADIUS = 90.f;
        if (dist <= PICKUP_RADIUS) {
            pickupPrompt = true;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                pickUp();
                std::cout << "Picked up " << passengerName << " for: " << destination 
                          << " | Fare: " << fare << " TK" << std::endl;
            }
        } else {
            pickupPrompt = false;
        }
    }
}

void Passenger::draw(sf::RenderWindow& window) {
    if (isPickedUp) return;

    float cy = y + 8.f; // ground line under feet
    float bob = std::sin(animTimer * 4.0f) * 1.2f;
    bool waving = pickupPrompt;

    // --- Locator beacon: makes the passenger easy to find on the big map
    float pulse = 0.5f + 0.5f * std::sin(animTimer * 3.0f);
    // Light column rising from the ground
    sf::RectangleShape beam(sf::Vector2f(6.f, 46.f));
    beam.setFillColor(sf::Color(255, 230, 120, (std::uint8_t)(60 + 50 * pulse)));
    beam.setPosition(sf::Vector2f(x - 3.f, cy - 52.f));
    window.draw(beam);
    // Pulsing ground ring
    sf::CircleShape ring(18.f + 8.f * pulse);
    ring.setFillColor(sf::Color(255, 240, 160, (std::uint8_t)(40 + 40 * pulse)));
    ring.setPosition(sf::Vector2f(x - (18.f + 8.f * pulse), cy - 12.f));
    window.draw(ring);

    // Shadow at feet
    shadow.setPosition(sf::Vector2f(x - 22.f, cy - 4.f));
    window.draw(shadow);

    // Legs (slight swing = standing/walking twitch)
    float legSwing = std::sin(animTimer * 4.0f + 0.6f) * 2.f;
    legL.setOrigin(sf::Vector2f(2.f, 0.f));
    legL.setPosition(sf::Vector2f(x - 4.f, cy - 14.f));
    legL.setRotation(sf::degrees(legSwing));
    window.draw(legL);
    legR.setOrigin(sf::Vector2f(2.f, 0.f));
    legR.setPosition(sf::Vector2f(x + 4.f, cy - 14.f));
    legR.setRotation(sf::degrees(-legSwing));
    window.draw(legR);

    // Torso
    float bodyTop = cy - 34.f + bob;
    torso.setOrigin(sf::Vector2f(6.f, 0.f));
    torso.setPosition(sf::Vector2f(x, bodyTop));
    window.draw(torso);

    // Left arm (down / slight sway)
    armL.setOrigin(sf::Vector2f(2.f, 2.f));
    armL.setPosition(sf::Vector2f(x - 6.f, bodyTop + 2.f));
    float armLrot = waving ? 150.f + std::sin(animTimer * 10.f) * 30.f : 8.f;
    armL.setRotation(sf::degrees(armLrot));
    window.draw(armL);

    // Right arm (waves when player is near)
    float armRrot = waving ? -150.f + std::sin(animTimer * 10.f) * 30.f : -8.f;
    armR.setOrigin(sf::Vector2f(2.f, 2.f));
    armR.setPosition(sf::Vector2f(x + 6.f, bodyTop + 2.f));
    armR.setRotation(sf::degrees(armRrot));
    window.draw(armR);

    // Head
    head.setPosition(sf::Vector2f(x - 7.f, bodyTop - 14.f));
    window.draw(head);

    // "!" prompt above head when near
    if (waving) {
        sf::CircleShape hint(5.f);
        hint.setFillColor(sf::Color(255, 240, 0));
        hint.setPosition(sf::Vector2f(x + 8.f, bodyTop - 26.f));
        window.draw(hint);
        sf::RectangleShape bar(sf::Vector2f(3.f, 9.f));
        bar.setFillColor(sf::Color(255, 240, 0));
        bar.setPosition(sf::Vector2f(x + 9.5f, bodyTop - 22.f));
        window.draw(bar);
    }
}

void Passenger::pickUp() {
    isPickedUp = true;
    pickupPrompt = false;
}

void Passenger::reset() {
    isPickedUp = false;
    pickupPrompt = false;
    animTimer = 0.0f;
}

int Passenger::getFare() const {
    return fare;
}

std::string Passenger::getDestination() const {
    return destination;
}

std::string Passenger::getPassengerName() const {
    return passengerName;
}

bool Passenger::isPicked() const {
    return isPickedUp;
}

void Passenger::setMissionId(int id) {
    missionId = id;
}

int Passenger::getMissionId() const {
    return missionId;
}