#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <SFML/Audio.hpp>
#include <array>
#include <memory>
#include <string>

namespace KUET {

    enum class SFX {
        Click,        // button / key click
        Success,      // login success / mission complete fanfare
        Error,        // login error / invalid action
        Pickup,       // passenger picked up
        Damage,       // collision / pothole hit
        Turbo,        // boost activation
        Complete,     // mission completed
        Count
    };

    class SoundManager {
    private:
        static std::array<sf::SoundBuffer, (size_t)SFX::Count> buffers;
        static std::array<std::unique_ptr<sf::Sound>, (size_t)SFX::Count> sounds;
        static bool initialized;
        static float masterVolume;

        // Looping ambient layers (engine / wind)
        static std::unique_ptr<sf::SoundBuffer> engineBuf;
        static std::unique_ptr<sf::Sound> engineSound;
        static bool enginePlaying;

        static std::unique_ptr<sf::SoundBuffer> windBuf;
        static std::unique_ptr<sf::Sound> windSound;
        static bool windPlaying;

        static float engineVolume();
        static float windVolume();
        static float musicVolume();

        // Background music (streamed from a file, e.g. mp3)
        static std::unique_ptr<sf::Music> music;

    public:
        static bool init();
        static void play(SFX sfx);
        static void setMasterVolume(float v);

        // Engine / ambient layers
        static void startEngine();
        static void stopEngine();
        static void setEngine(float speedFrac, bool boosting);
        static void startWind();
        static void stopWind();

        // Background music
        static bool startMusic(const std::string& path);
        static void stopMusic();
        static void setMusicPaused(bool paused);
        static bool isMusicPlaying();
    };
}

#endif
