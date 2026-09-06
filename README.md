# KUET Rickshaw Rush 🛺

**A neon-noir, full-campus rickshaw driving game built with C++ + SFML.**

> You are the fastest rickshaw chacha on KUET campus. Pick up passengers,
> outsmart the clock, keep the old engine alive — and become the campus legend.

![Login Screen](screenshots/01_login_screen.png)
![Dashboard](screenshots/02_driver_dashboard.png)

---

## 📖 Table of Contents

- [About the Game](#-about-the-game)
- [Screenshots](#-screenshots)
- [Features](#-features)
- [How to Play](#-how-to-play)
- [Controls](#-controls)
- [The Missions](#-the-missions)
- [Garage & Upgrades](#-garage--upgrades)
- [Installation (Windows)](#-installation-windows)
- [Project Structure](#-project-structure)
- [Tech Stack](#-tech-stack)
- [Coming Soon](#-coming-soon)

---

## 🎮 About the Game

KUET Rickshaw Rush drops you into a complete, hand-drawn **2000 × 1500** bird's-eye
map of the Khulna University of Engineering & Technology campus — every hall,
academic building, hostel, garden, pond and historic gate included.

You drive a lovingly-detailed pedal rickshaw (animated pedals, spinning spoke
wheels, suspension, a waving canopy and a headlight that shines in the dark).
Pick up passengers, ferry them across campus to their destinations, and earn TK
to upgrade your ride into a serious machine.

Deliver with style: finish fast, avoid scratches, keep the tank above the fuel
line, and don't touch the boost button on missions that forbid it.

---

## 📸 Screenshots

| Screen | Preview |
|---|---|
| **Driver Portal (Login / Register)** | ![Login](screenshots/01_login_screen.png) |
| **Driver Dashboard** | ![Dashboard](screenshots/02_driver_dashboard.png) |
| **Garage / Upgrades** | ![Garage](screenshots/03_garage_upgrades.png) |
| **Mission 1 — Campus Map** | ![Gameplay](screenshots/04_gameplay_map.png) |
| **On the road** | ![Driving](screenshots/05_gameplay_driving.png) |

> Tip: you can press **F12** in-game anytime to save your own screenshots as PNGs
> right into the game folder.

---

## ✨ Features

- **Full KUET campus map** — hostels, academic blocks, central library, Shaheed
  Minar, July Chattar, gardens, ponds, play-fields, street lamps, benches and
  all four campus gates drawn as an interconnected road network.
- **5 story-driven passenger missions** with escalating difficulty and bonus
  requirements (no-damage, fuel-minimum, no-boost).
- **Realistic rickshaw** — animated pedals, 4-spoke wheels spinning with speed,
  suspension bob, smooth facing rotation, roof canopy and a directional headlight.
- **Realistic passengers** — idle bobbing, waving arms, swinging legs, and a
  golden beacon + "PRESS SPACE" prompt so you never lose them.
- **Neon cyberpunk UI** — glowing login portal, dashboard and garage with a
  pulsing neon aesthetic over the campus background art.
- **Garage upgrades** — speed, fuel tank, armor, turbo, and 5 paint schemes.
  Upgrades persist to disk between sessions.
- **Soft road-following** — the rickshaw gently hugs the road network while you
  keep full control.
- **Day / Night cycle** — press `U` to cycle morning → afternoon → night, complete
  with animated lighting and lamp flicker.
- **Live sound** — synthesized engine rumble & wind noise tied to your speed,
  pickup/finish/damage SFX, and a streaming background music track.
- **Minimap + compass** — a live campus minimap and a **gold directional arrow**
  that always points to the drop zone, with live distance.
- **Pause menu & help** — freeze the game (P/Esc), jump back to the dashboard,
  or open a full controls cheat-sheet.
- **Custom Mission Builder** — press `C` on the Dashboard to design your own
  ride: pick any pickup point and any destination on the campus map, and set
  your own fare and time limit.
- **Custom exception handling** — clean gameplay events for out-of-fuel and
  time-up, with an in-game "repair me!" prompt.

---

## 🕹️ How to Play

1. **Log in or register** on the Driver Portal. Your earnings and upgrades are
   saved to your account.
2. From the **Dashboard**, read the mission cards and press a number key `1–5`
   to accept a fare.
3. The chosen passenger spawns as a **golden beacon** on campus. Drive up to them
   (you'll see **PRESS SPACE**), and press **SPACE** to pick them up — the clock
   starts.
4. A **gold compass arrow** now points toward the destination. Reach it before
   the timer runs out, watching your **FUEL** and **HULL** bars on the HUD.
5. Reach the glowing **DROP POINT** to complete the mission and earn TK.
6. Spend your TK in the **Garage** (`U` on the dashboard) and take on the next fare!

**Make your own ride (Custom Mission Builder):** from the Dashboard press `C`.
A scaled campus map opens with a crosshair — move it with **WASD/Arrows**, press
**SPACE** to drop the pickup point, then **SPACE** again for the destination
(both snap onto the nearest road automatically). Tune the **fare** with `-`/`+`
and the **time limit** with `[`/`]`, then press **ENTER** to start your custom
ride, or `Esc` to back out. It's a one-shot fare — finish it and you're back at
the Dashboard.

**Mission requirements matter:** some fares refuse damaged rides
(no damage), demand the tank stays above a percentage, or forbid turbo-boosting.
Fail any requirement and the mission fails — plan your route carefully.

Don't miss: each destination is placed **on or beside the road network** — the
map is fully drivable from end to end.

---

## ⌨️ Controls

| Key | Action |
|---|---|
| `W` `A` `S` `D` or **Arrow Keys** | Move the rickshaw |
| `SHIFT` | Boost (faster, burns more fuel) |
| `SPACE` | Pick up / drop off passenger |
| `R` | Repair rickshaw (fuel + hull) |
| `U` | Change time of day (morning / afternoon / night) |
| `P` or `Esc` | Pause menu (Resume / Back / Quit) |
| `B` | Back to Dashboard *(from the pause menu)* |
| `Q` | Quit game *(from the pause menu)* |
| `H` | Show / hide in-game help & controls |
| `T` | Turbo *(unlocks from Mission 4 — spend TK to own it)* |
| `F12` | Save a screenshot as PNG |

**Dashboard:**
| Key | Action |
|---|---|
| `1` – `5` | Start mission 1 – 5 |
| `U` / `G` | Open the Garage / Upgrades shop |
| `C` | Open the Custom Mission Builder |
| `Esc` | Back to login |

**Custom Mission Builder:**
| Key | Action |
|---|---|
| `W` `A` `S` `D` / **Arrows** | Move the crosshair |
| `SPACE` | Set pickup point, then destination |
| `-` / `+` | Lower / raise the fare |
| `[` / `]` | Lower / raise the time limit |
| `ENTER` | Start the custom ride |
| `Esc` / `C` | Back to Dashboard |

---

## 🚖 The Missions

| # | Rider | Route | Fare | Time | Bonus Rules |
|---|---|---|---|---|---|
| 1 | Rahim (CSE Student) | Main Gate → CSE Building | 50 TK | 60 s | — |
| 2 | Sultana (Library) | CSE → KUET Library | 75 TK | 50 s | No Damage |
| 3 | Shahid (Student) | Library → Shaheed Minar | 60 TK | 55 s | Fuel > 50% |
| 4 | Liton (Hall Student) | Shaheed Minar → Amar Ekushey Hall | 80 TK | 40 s | No Boost |
| 5 | Gate Guard | Amar Ekushey Hall → KUET Main Gate | 150 TK | 70 s | No Damage, Fuel > 40%, No Boost |

---

## ⚙️ Garage & Upgrades

Spend the TK you earn across a mission to make the rickshaw a monster:

| Upgrade | Level 1 | Level 2 | Level 3 |
|---|---|---|---|
| **Speed** | 300 TK | 500 TK | 800 TK |
| **Fuel tank** | 200 TK | 350 TK | 600 TK |
| **Armor (hull)** | 250 TK | 400 TK | 650 TK |

- **Turbo** — 1000 TK (once). Activates with `T` from Mission 4.
- **Paint** — 150 TK each, 5 schemes.

Everything is saved to your account (`users.txt`) and re-applied automatically
every time you start a mission.

---

## 💻 Installation (Windows)

> Built and tested with **MSYS2 / UCRT64**, **GCC** and **SFML 3.0.2**.

### 1. Install MSYS2

Download and install [MSYS2](https://www.msys2.org/). Open the **UCRT64** terminal
and update:

```bash
pacman -Syu
```

### 2. Install the toolchain + SFML

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-sfml
```

### 3. Build the game

From the project folder (UCRT64 terminal or PowerShell with the MSYS2 paths):

```bash
g++ -std=c++17 -Iinclude -IC:/msys64/ucrt64/include \
    src/main.cpp src/Auth.cpp src/Campus.cpp src/Passenger.cpp src/Rickshaw.cpp \
    src/Environment.cpp src/Constants.cpp src/SoundManager.cpp \
    -o game.exe -LC:/msys64/ucrt64/lib \
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

### 4. Run

```bash
./game.exe
```

> The game expects the `assets/` folder (background art, fonts, music) in the
> same directory as the executable.

---

## 📂 Project Structure

```
├── src/
│   ├── main.cpp           # Game loop, world, missions, HUD, compass, screenshots
│   ├── Auth.cpp           # Login / Dashboard / Custom Mission Builder / Garage
│   ├── Campus.cpp         # Shared campus map: buildings & road network
│   ├── Rickshaw.cpp       # Realistic rickshaw rendering + movement
│   ├── Passenger.cpp      # Passenger characters + pickup logic
│   ├── Environment.cpp    # Grass, ponds, gardens, day/night
│   ├── Constants.cpp      # Global constants
│   └── SoundManager.cpp   # Engine/wind loops, SFX, background music
├── include/
│   ├── Auth.h / Missions.h / Rickshaw.h / Passenger.h
│   ├── Environment.h / Minimap.h / GameObject.h / Vehicle.h
│   ├── Interfaces.h / CustomExceptions.h / SoundManager.h / Constants.h
│   └── Campus.h           # Campus geometry shared with the mission builder
├── assets/
│   ├── fonts/             # arial.ttf, seguiemj.ttf
│   ├── bg music.mp3       # streaming background music
│   ├── background.png     # neon background art
│   └── lv1.tmx
└── *.png                  # game screenshots
```

---

## 🧰 Tech Stack

- **C++17**
- **SFML 3.0.2** — graphics, window, system, audio
- **MSYS2 / UCRT64 (GCC)** toolchain
- Procedural visuals — the entire campus and rickshaw are drawn with SFML shapes
  (no external sprite sheets), synthesized audio for the engine & wind.

---

## 🚀 Coming Soon

- More missions & passenger stories
- Endless / free-roam mode beyond custom rides
- Save & re-run your favourite custom routes
- Traffic, day/night pedestrian flow
- Leaderboards & per-account high scores
- Sound options in the settings menu
- Mobile build

---

## ©️ Credits

Built with lots of repair rides and even more cups of cha. KUET Rickshaw Rush is
a fan-made love letter to campus life — used only for learning and fun.

**Drive safe — and keep the pedals spinning.** 🛺💨
