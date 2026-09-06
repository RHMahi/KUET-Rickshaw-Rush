#include "../include/Rickshaw.h"
#include "../include/CustomExceptions.h"
#include <cmath>
#include <iostream>

using namespace KUET;
using namespace std;

Rickshaw::Rickshaw(float sx, float sy) 
    : Vehicle(sx, sy), hasTurbo(false), isMoving(false), 
      wobbleTimer(0.0f), wobbleAngle(0.0f), wheelOffset(0.0f),
      pedalAngle(0.0f), bobPhase(0.0f) {

    // Carriage body (cycle-rickshaw box)
    body.setSize(sf::Vector2f(68, 38));
    body.setFillColor(sf::Color(255, 215, 0));
    body.setOutlineThickness(2);
    body.setOutlineColor(sf::Color(80, 60, 20));

    // Wheels
    wheel1.setRadius(14.f);
    wheel1.setFillColor(sf::Color(30, 30, 30));
    wheel1.setOutlineThickness(3);
    wheel1.setOutlineColor(sf::Color(120, 120, 120));
    wheel2.setRadius(14.f);
    wheel2.setFillColor(sf::Color(30, 30, 30));
    wheel2.setOutlineThickness(3);
    wheel2.setOutlineColor(sf::Color(120, 120, 120));

    // Wheel hubs (small silver centers)
    wheelHub1.setRadius(3.f);
    wheelHub1.setFillColor(sf::Color(220, 220, 220));
    wheelHub2.setRadius(3.f);
    wheelHub2.setFillColor(sf::Color(220, 220, 220));

    // Build 4 spokes per wheel (rotate around hub)
    auto makeSpokes = [&](std::vector<sf::RectangleShape>& arr) {
        arr.resize(4);
        for (int i = 0; i < 4; ++i) {
            arr[i].setSize(sf::Vector2f(2.f, 22.f));
            arr[i].setFillColor(sf::Color(190, 190, 190));
            arr[i].setOrigin(sf::Vector2f(1.f, 11.f));
            arr[i].setRotation(sf::degrees(i * 45.f));
        }
    };
    makeSpokes(spokes1);
    makeSpokes(spokes2);

    // Roof canopy
    roof.setSize(sf::Vector2f(52, 24));
    roof.setFillColor(sf::Color(100, 150, 200));
    roof.setOutlineThickness(1);
    roof.setOutlineColor(sf::Color(20, 40, 60));

    // Hood (front glance / pillion seat cover)
    hood.setSize(sf::Vector2f(24, 14));
    hood.setFillColor(sf::Color(200, 150, 50));
    hood.setOutlineThickness(1);
    hood.setOutlineColor(sf::Color(80, 50, 10));

    // Chassis bar (main backbone from rear to front)
    frame.setSize(sf::Vector2f(82.f, 4.f));
    frame.setFillColor(sf::Color(60, 60, 70));
    frame.setOutlineThickness(1);
    frame.setOutlineColor(sf::Color(10, 10, 14));

    // Driver seat (bicycle saddle)
    seat.setRadius(5.f);
    seat.setFillColor(sf::Color(40, 40, 45));
    seat.setOutlineThickness(1);
    seat.setOutlineColor(sf::Color(0, 0, 0));

    // Front fork (steering column)
    fork.setSize(sf::Vector2f(4.f, 30.f));
    fork.setFillColor(sf::Color(70, 70, 85));
    fork.setOutlineThickness(1);
    fork.setOutlineColor(sf::Color(10, 10, 14));

    // Handlebar
    handlebar.setSize(sf::Vector2f(26.f, 3.f));
    handlebar.setFillColor(sf::Color(120, 120, 130));

    // Pedal crank + pedal
    pedalCrank.setSize(sf::Vector2f(2.f, 16.f));
    pedalCrank.setFillColor(sf::Color(150, 150, 160));
    pedalCrank.setOrigin(sf::Vector2f(1.f, 8.f));
    pedal.setSize(sf::Vector2f(12.f, 3.f));
    pedal.setFillColor(sf::Color(90, 90, 100));

    headlight.setRadius(5.f);
    headlight.setFillColor(sf::Color(255, 255, 210));

    updatePosition();
}

void Rickshaw::updatePosition() {
    float bob = std::sin(bobPhase) * 1.2f; // subtle suspension bob
    // The rickshaw's pivot (x,y) is the rear axle. Everything is placed relative to it.
    body.setPosition(sf::Vector2f(x - 34.f, y - 26.f - bob));
    frame.setPosition(sf::Vector2f(x - 34.f, y - 2.f));
    seat.setPosition(sf::Vector2f(x + 30.f, y - 10.f));
    fork.setPosition(sf::Vector2f(x + 40.f, y - 24.f));
    handlebar.setPosition(sf::Vector2f(x + 52.f, y - 14.f));
    roof.setPosition(sf::Vector2f(x - 34.f, y - 40.f));
    hood.setPosition(sf::Vector2f(x - 4.f, y - 34.f));

    wheel1.setPosition(sf::Vector2f(x - 20.f, y));
    wheel2.setPosition(sf::Vector2f(x + 20.f, y));
    wheelHub1.setPosition(sf::Vector2f(x - 20.f, y));
    wheelHub2.setPosition(sf::Vector2f(x + 20.f, y));

    headlight.setPosition(sf::Vector2f(x + 42.f, y - 22.f));
}

Rickshaw& Rickshaw::operator+(string upgrade) {
    if (upgrade == "Turbo") {
        hasTurbo = true;
        body.setFillColor(sf::Color(255, 100, 50));
        roof.setFillColor(sf::Color(200, 50, 50));
    }
    return *this;
}

void Rickshaw::resetVisuals() {
    body.setFillColor(sf::Color(255, 215, 0));
    roof.setFillColor(sf::Color(100, 150, 200));
    hood.setFillColor(sf::Color(200, 150, 50));
}

void Rickshaw::applyPaint(int index) {
    switch (index) {
        case 0: // Classic Yellow
            body.setFillColor(sf::Color(255, 215, 0));
            roof.setFillColor(sf::Color(100, 150, 200));
            hood.setFillColor(sf::Color(200, 150, 50));
            break;
        case 1: // Crystal Blue
            body.setFillColor(sf::Color(60, 160, 255));
            roof.setFillColor(sf::Color(200, 230, 250));
            hood.setFillColor(sf::Color(40, 90, 160));
            break;
        case 2: // Racing Red
            body.setFillColor(sf::Color(235, 60, 60));
            roof.setFillColor(sf::Color(255, 230, 190));
            hood.setFillColor(sf::Color(150, 30, 30));
            break;
        case 3: // Royal Gold
            body.setFillColor(sf::Color(233, 150, 104));
            roof.setFillColor(sf::Color(255, 255, 200));
            hood.setFillColor(sf::Color(190, 110, 60));
            break;
        default: // Night Shadow
            body.setFillColor(sf::Color(70, 70, 90));
            roof.setFillColor(sf::Color(150, 160, 190));
            hood.setFillColor(sf::Color(45, 45, 60));
            break;
    }
}

void Rickshaw::update(float dt) {
    isMoving = false;
    float currentSpeed = speed * speedMult * (hasTurbo ? 2.5f : 1.0f);
    float dx = 0, dy = 0;
    int direction = 0;

    // Boost check for tracking
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
        currentSpeed *= 1.5f;
        fuel -= 1.5f * dt;
        if (fuel < 0) fuel = 0;
        if (boostCallback) boostCallback();
    }

    // Movement (WASD + arrows)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    { dy -= currentSpeed * dt; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  { dy += currentSpeed * dt; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  { dx -= currentSpeed * dt; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) { dx += currentSpeed * dt; isMoving = true; }

    if (dx != 0 && dy != 0) {
        // normalize diagonal speed
        float inv = 0.70710678f;
        dx *= inv; dy *= inv;
    }

    if (isMoving) {
        fuel -= 2.0f * dt;
        if (fuel < 0) fuel = 0;
        // Animate pedals + wheel bob only while driving
        float pedalRate = 6.0f + currentSpeed * 0.05f;
        pedalAngle += pedalRate * dt;
        bobPhase += dt * 6.0f;
    }

    float newX = x + dx;
    float newY = y + dy;

    // KUET Campus bounds
    newX = std::max(80.f, std::min(1920.f, newX));
    newY = std::max(80.f, std::min(1440.f, newY));

    x = newX;
    y = newY;

    // Smooth facing angle toward movement direction
    if (isMoving && (dx != 0 || dy != 0)) {
        float targetAngle = std::atan2(dy, dx) * 180.0f / 3.14159f;
        float diff = targetAngle - wobbleAngle;
        while (diff > 180.f) diff -= 360.f;
        while (diff < -180.f) diff += 360.f;
        wobbleAngle += diff * std::min(1.f, dt * 8.f); // smooth turn
        wobbleTimer += dt * 12.f;
    }

    // Wheel vertical offset while going up/down (fake "hill")
    if (dy < 0) wheelOffset = std::max(wheelOffset - 10.0f * dt, -10.0f);
    else if (dy > 0) wheelOffset = std::min(wheelOffset + 10.0f * dt, 10.0f);
    else {
        if (wheelOffset > 0) wheelOffset = std::max(wheelOffset - 5.0f * dt, 0.0f);
        if (wheelOffset < 0) wheelOffset = std::min(wheelOffset + 5.0f * dt, 0.0f);
    }

    updatePosition();
}


void Rickshaw::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;

    if (damageCallback) damageCallback(amount);
}

void Rickshaw::setDamageCallback(std::function<void(int)> callback) {
    damageCallback = callback;
}

void Rickshaw::setBoostCallback(std::function<void()> callback) {
    boostCallback = callback;
}

int Rickshaw::CalculateFare() {
    return 30;
}

void Rickshaw::draw(sf::RenderWindow& window) {
    float facing = wobbleAngle; // degrees

    // --- 1. Shadow under the rickshaw (flattened ellipse)
    sf::CircleShape shadow(18.f);
    shadow.setScale(sf::Vector2f(1.7f, 0.5f));
    shadow.setFillColor(sf::Color(0, 0, 0, 60));
    shadow.setPosition(sf::Vector2f(x - 20.f, y + 16.f));
    window.draw(shadow);

    // Build a transform that rotates everything around the rickshaw pivot (x,y)
    sf::Transform t;
    t.rotate(sf::degrees(facing), sf::Vector2f(x, y));

    // --- 2. Rear wheel + rotating spokes
    sf::Transform w1t = t;
    w1t.rotate(sf::degrees(pedalAngle * 1.6f), sf::Vector2f(x - 20.f, y));
    for (int i = 0; i < 4; ++i) {
        spokes1[i].setPosition(sf::Vector2f(x - 20.f, y));
        spokes1[i].setRotation(sf::degrees(i * 45.f + pedalAngle * 1.6f));
    }
    window.draw(wheel1, w1t);
    for (auto& s : spokes1) window.draw(s, t);
    // hub
    sf::CircleShape hub1(4.f);
    hub1.setFillColor(sf::Color(220, 220, 220));
    hub1.setPosition(sf::Vector2f(x - 22.f, y - 2.f));
    window.draw(hub1, t);

    // --- 3. Chassis / frame / seat / body
    window.draw(frame, t);
    window.draw(seat, t);
    window.draw(body, t);

    // --- 4. Pedal crank + pedal (rotating)
    sf::Transform pt = t;
    pt.rotate(sf::degrees(pedalAngle * 3.f), sf::Vector2f(x + 30.f, y));
    sf::RectangleShape crank(sf::Vector2f(2.f, 14.f));
    crank.setOrigin(sf::Vector2f(1.f, 7.f));
    crank.setFillColor(sf::Color(150, 150, 160));
    crank.setPosition(sf::Vector2f(x + 30.f, y - 2.f));
    window.draw(crank, pt);
    sf::RectangleShape pedalBlock(sf::Vector2f(12.f, 3.f));
    pedalBlock.setOrigin(sf::Vector2f(6.f, 1.f));
    pedalBlock.setFillColor(sf::Color(90, 90, 100));
    pedalBlock.setPosition(sf::Vector2f(x + 30.f, y - 2.f));
    window.draw(pedalBlock, pt);

    // --- 5. Front fork + handlebar
    window.draw(fork, t);
    window.draw(handlebar, t);
    window.draw(hood, t);

    // --- 6. Roof canopy
    window.draw(roof, t);

    // --- 7. Front wheel + rotating spokes
    sf::Transform w2t = t;
    w2t.rotate(sf::degrees(pedalAngle * 1.6f), sf::Vector2f(x + 20.f, y));
    for (int i = 0; i < 4; ++i) {
        spokes2[i].setPosition(sf::Vector2f(x + 20.f, y));
        spokes2[i].setRotation(sf::degrees(i * 45.f + pedalAngle * 1.6f));
    }
    window.draw(wheel2, w2t);
    for (auto& s : spokes2) window.draw(s, t);
    sf::CircleShape hub2(4.f);
    hub2.setFillColor(sf::Color(220, 220, 220));
    hub2.setPosition(sf::Vector2f(x + 18.f, y - 2.f));
    window.draw(hub2, t);

    // --- 8. Headlight + beam (snapped to cardinal directions for a stable beam)
    float lightAngle = 0.f;
    if (facing >= -45.f && facing < 45.f)          lightAngle = 0.f;   // right
    else if (facing >= 45.f && facing < 135.f)     lightAngle = 90.f;  // down
    else if (facing >= 135.f || facing <= -135.f)  lightAngle = 180.f; // left
    else                                           lightAngle = -90.f; // up

    float headX = x, headY = y - 6.f;
    float beamX = x, beamY = y - 6.f;
    switch ((int)(lightAngle + 90.f) / 90) {
        case 0:  headX = x + 40.f; beamX = x + 24.f;  break; // right
        case 1:  headY = y + 20.f; beamY = y + 18.f;  break; // down
        case 2:  headX = x - 30.f; beamX = x - 16.f;  break; // left
        default: headY = y - 30.f; beamY = y - 28.f;  break; // up
    }

    sf::CircleShape hl(5.f);
    hl.setFillColor(sf::Color(255, 230, 150));
    hl.setPosition(sf::Vector2f(headX - 5.f, headY - 5.f));
    window.draw(hl, t);

    sf::ConvexShape beam;
    beam.setPointCount(3);
    beam.setPoint(0, sf::Vector2f(0.f, 0.f));
    beam.setPoint(1, sf::Vector2f(100.f, -32.f));
    beam.setPoint(2, sf::Vector2f(100.f, 32.f));
    beam.setFillColor(sf::Color(255, 255, 170, 70));
    beam.setPosition(sf::Vector2f(beamX, beamY));
    beam.setRotation(sf::degrees(lightAngle));
    window.draw(beam, t);
}