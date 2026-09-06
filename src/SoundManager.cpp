#include "../include/SoundManager.h"
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

namespace {
    constexpr double kPi = 3.14159265358979323846;

    // Build a mono PCM buffer from a set of "notes". Each note is (freq, duration).
    // A simple envelope and optional harmonics make it sound pleasant rather than harsh.
    sf::SoundBuffer buildArpeggio(const double* freqs, int noteCount, double noteDur) {
        constexpr int SR = 44100;
        std::vector<std::int16_t> all;
        for (int n = 0; n < noteCount; ++n) {
            int count = (int)(SR * noteDur);
            for (int i = 0; i < count; ++i) {
                double t = (double)i / SR;
                double p = (double)i / count;
                double env = std::exp(-4.0 * p);
                double attack = std::min(1.0, (double)(i % (int)(SR * 0.012)) / (SR * 0.012));
                double val = std::sin(2.0 * kPi * freqs[n] * t)
                           + 0.35 * std::sin(4.0 * kPi * freqs[n] * t)
                           + 0.15 * std::sin(6.0 * kPi * freqs[n] * t);
                all.push_back((std::int16_t)(val * env * attack * 32000.0));
            }
        }
        sf::SoundBuffer buf;
        (void)buf.loadFromSamples(all.data(), (std::uint64_t)all.size(), 1, SR, {sf::SoundChannel::Mono});
        return buf;
    }

    // Build a mono buffer driven by the callback
    sf::SoundBuffer buildMono(const std::function<void(double t, double p, double &v)>& synth, double dur) {
        constexpr int SR = 44100;
        int count = (int)(SR * dur);
        std::vector<std::int16_t> samples;
        samples.reserve((size_t)count);
        for (int i = 0; i < count; ++i) {
            double t = (double)i / SR;
            double p = (double)i / count;
            double v = 0.0;
            synth(t, p, v);
            double attack = std::min(1.0, (double)(i % (int)(SR * 0.015)) / (SR * 0.015));
            samples.push_back((std::int16_t)(v * attack * 30000.0));
        }
        sf::SoundBuffer buf;
        (void)buf.loadFromSamples(samples.data(), (std::uint64_t)samples.size(), 1, SR, {sf::SoundChannel::Mono});
        return buf;
    }
}

namespace KUET {

    std::array<sf::SoundBuffer, (size_t)SFX::Count> SoundManager::buffers;
    std::array<std::unique_ptr<sf::Sound>, (size_t)SFX::Count> SoundManager::sounds;
    bool SoundManager::initialized = false;
    float SoundManager::masterVolume = 80.f;

    std::unique_ptr<sf::SoundBuffer> SoundManager::engineBuf;
    std::unique_ptr<sf::Sound> SoundManager::engineSound;
    bool SoundManager::enginePlaying = false;

    std::unique_ptr<sf::SoundBuffer> SoundManager::windBuf;
    std::unique_ptr<sf::Sound> SoundManager::windSound;
    bool SoundManager::windPlaying = false;

    std::unique_ptr<sf::Music> SoundManager::music;

    // Continuous low rumble for the engine. Uses whole cycles so it loops seamlessly.
    sf::SoundBuffer buildEngineLoop() {
        constexpr int SR = 44100;
        constexpr double baseFreq = 55.0;        // ~55Hz rumble
        constexpr double dur = 1.0;              // 1 second
        constexpr int cycles = (int)std::round(baseFreq * dur); // whole-number phase wrap
        int count = (int)(SR * dur);
        std::vector<std::int16_t> samples;
        samples.reserve((size_t)count);
        static unsigned long seed = 777;
        for (int i = 0; i < count; ++i) {
            double t = (double)i / SR;
            double theta = 2.0 * kPi * ((double)cycles * t / dur); // wraps to 0 at end
            // low sawtooth-ish rumble with a couple of harmonics
            double v = 0.55 * std::sin(theta)
                     + 0.28 * std::sin(2.0 * theta)
                     + 0.14 * std::sin(4.0 * theta);
            // subtle combustion noise, lowpassed via averaging
            seed = seed * 1103515245 + 12345;
            double n = ((seed >> 8) & 0xFFFF) / (double)0xFFFF * 2.0 - 1.0;
            v += 0.10 * n;
            // slow amplitude wobble (engine "lope")
            v *= 0.85 + 0.15 * std::sin(2.0 * kPi * 4.0 * t);
            samples.push_back((std::int16_t)(v * 32000.0));
        }
        sf::SoundBuffer buf;
        (void)buf.loadFromSamples(samples.data(), (std::uint64_t)samples.size(), 1, SR, {sf::SoundChannel::Mono});
        return buf;
    }

    // Airy wind/road noise with a gentle fade at both ends to avoid seam clicks.
    sf::SoundBuffer buildWindLoop() {
        constexpr int SR = 44100;
        constexpr double dur = 0.9;
        int count = (int)(SR * dur);
        std::vector<std::int16_t> samples;
        samples.reserve((size_t)count);
        static unsigned long seed = 4242;
        double prev = 0.0;
        for (int i = 0; i < count; ++i) {
            double p = (double)i / count;
            seed = seed * 1103515245 + 12345;
            double white = ((seed >> 16) & 0x7FFF) / (double)0x7FFF * 2.0 - 1.0;
            // crude lowpass (warmer "whoosh"), then add a pass of high freq
            double low = 0.15 * white + 0.85 * prev;
            prev = low;
            double v = 0.7 * low + 0.3 * white;
            // crossfade seam: ramp in then out
            double fade = std::min(p * 40.0, (1.0 - p) * 40.0);
            fade = std::max(0.0, std::min(1.0, fade));
            samples.push_back((std::int16_t)(v * fade * 26000.0));
        }
        sf::SoundBuffer buf;
        (void)buf.loadFromSamples(samples.data(), (std::uint64_t)samples.size(), 1, SR, {sf::SoundChannel::Mono});
        return buf;
    }

    bool SoundManager::init() {
        if (initialized) return true;

        // Click: short high blip
        buffers[(size_t)SFX::Click] = buildMono([&](double t, double p, double& v) {
            v = std::sin(2.0 * kPi * 1600.0 * t) * std::exp(-25.0 * p);
        }, 0.05);

        // Success: ascending 3-note arpeggio (C5-E5-G5)
        double succNotes[3] = {523.25, 659.25, 783.99};
        buffers[(size_t)SFX::Success] = buildArpeggio(succNotes, 3, 0.18);

        // Error: low square-wave buzz
        buffers[(size_t)SFX::Error] = buildMono([&](double t, double p, double& v) {
            double sq = (std::sin(2.0 * kPi * 160.0 * t) > 0.0) ? 1.0 : -1.0;
            v = sq * std::exp(-3.0 * p) * 0.8;
        }, 0.35);

        // Pickup: cheerful single ding (G5)
        buffers[(size_t)SFX::Pickup] = buildMono([&](double t, double p, double& v) {
            v = std::sin(2.0 * kPi * 784.0 * t) * std::exp(-8.0 * p);
        }, 0.25);

        // Damage: low descending thump
        buffers[(size_t)SFX::Damage] = buildMono([&](double t, double p, double& v) {
            double f = 90.0 * (1.0 - 0.5 * p);
            v = std::sin(2.0 * kPi * f * t) * std::exp(-6.0 * p);
        }, 0.2);

        // Turbo: rising whoosh (sine + noise)
        buffers[(size_t)SFX::Turbo] = buildMono([&](double t, double p, double& v) {
            double f = 220.0 * (1.0 + 2.5 * p);
            static unsigned long seed = 12345;
            seed = seed * 1103515245 + 12345;
            double noise = ((seed >> 16) & 0x7FFF) / (double)0x7FFF * 2.0 - 1.0;
            v = (0.4 * std::sin(2.0 * kPi * f * t) + 0.6 * noise) * std::exp(-2.0 * p);
        }, 0.4);

        // Complete: 4-note fanfare (C5-E5-G5-C6)
        double compNotes[4] = {523.25, 659.25, 783.99, 1046.50};
        buffers[(size_t)SFX::Complete] = buildArpeggio(compNotes, 4, 0.16);

        // Set up sound objects (SFML 3.0 Sound has no default ctor)
        for (size_t i = 0; i < (size_t)SFX::Count; ++i) {
            sounds[i] = std::make_unique<sf::Sound>(buffers[i]);
            sounds[i]->setVolume(masterVolume);
        }

        initialized = true;
        return true;
    }

    void SoundManager::play(SFX sfx) {
        if (!initialized) init();
        sounds[(size_t)sfx]->play();
    }

    void SoundManager::setMasterVolume(float v) {
        masterVolume = v;
        for (size_t i = 0; i < (size_t)SFX::Count; ++i) {
            if (sounds[i]) sounds[i]->setVolume(v);
        }
        if (engineSound) engineSound->setVolume(engineVolume());
        if (windSound) windSound->setVolume(windVolume());
        if (music) music->setVolume(musicVolume());
    }

    // --- Engine / ambient layers ---

    float SoundManager::engineVolume() {
        return masterVolume * 0.45f;  // engine sits at 45% of master
    }

    float SoundManager::windVolume() {
        return masterVolume * 0.30f;  // wind sits at 30% of master
    }

    void SoundManager::startEngine() {
        if (!initialized) init();
        if (!engineBuf) {
            engineBuf = std::make_unique<sf::SoundBuffer>(buildEngineLoop());
            engineSound = std::make_unique<sf::Sound>(*engineBuf);
            engineSound->setLooping(true);
        }
        if (!enginePlaying) {
            engineSound->play();
            enginePlaying = true;
        }
        engineSound->setVolume(engineVolume());
        engineSound->setPitch(1.0f);
    }

    void SoundManager::stopEngine() {
        if (enginePlaying && engineSound) engineSound->stop();
        enginePlaying = false;
    }

    void SoundManager::setEngine(float speedFrac, bool boosting) {
        if (!initialized) init();
        if (speedFrac <= 0.001f) {
            stopEngine();
            return;
        }
        if (!engineSound || !engineBuf) startEngine();
        // Pitch rises with speed; boost pushes it higher and louder
        float basePitch = 0.9f + speedFrac * 0.6f;
        float pitch = boosting ? basePitch * 1.28f : basePitch;
        engineSound->setPitch(pitch);
        engineSound->setVolume(engineVolume() * (boosting ? 1.25f : 1.0f));
    }

    void SoundManager::startWind() {
        if (!initialized) init();
        if (!windBuf) {
            windBuf = std::make_unique<sf::SoundBuffer>(buildWindLoop());
            windSound = std::make_unique<sf::Sound>(*windBuf);
            windSound->setLooping(true);
        }
        if (!windPlaying) {
            windSound->play();
            windPlaying = true;
        }
    }

    void SoundManager::stopWind() {
        if (windPlaying && windSound) windSound->stop();
        windPlaying = false;
    }

    // --- Background music ---

    float SoundManager::musicVolume() {
        return masterVolume * 0.55f;  // music sits at 55% of master
    }

    bool SoundManager::startMusic(const std::string& path) {
        if (!initialized) init();
        if (!music) {
            music = std::make_unique<sf::Music>();
            if (!music->openFromFile(path)) {
                music.reset();
                return false;
            }
            music->setLooping(true);
            music->setVolume(musicVolume());
        }
        if (music->getStatus() != sf::SoundSource::Status::Playing) {
            music->play();
        }
        return true;
    }

    void SoundManager::stopMusic() {
        if (music && music->getStatus() != sf::SoundSource::Status::Stopped)
            music->stop();
    }

    void SoundManager::setMusicPaused(bool paused) {
        if (!music) return;
        if (paused && music->getStatus() == sf::SoundSource::Status::Playing)
            music->pause();
        else if (!paused && music->getStatus() == sf::SoundSource::Status::Paused)
            music->play();
    }

    bool SoundManager::isMusicPlaying() {
        return music && music->getStatus() == sf::SoundSource::Status::Playing;
    }
}
