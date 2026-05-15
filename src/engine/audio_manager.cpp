#include "audio_manager.hpp"

namespace engine {

auto AudioManager::instance() -> AudioManager& {
    static AudioManager instance;
    return instance;
}

auto AudioManager::update() const -> void {
    if (this->current_sound.has_value()) {
        UpdateMusicStream(this->current_sound.value());
    }
}

auto AudioManager::play(const parser::AudioPlay& audio_info) -> void {
    if (this->current_sound.has_value()) {
        this->stop();
    }

    auto music_it{this->music_cache.find(audio_info.path)};

    if (music_it == this->music_cache.end()) {
        if (!std::filesystem::exists(audio_info.path)) {
            throw std::runtime_error{
              "Audio file does not exist: " + audio_info.path.string()
            };
        }

        auto music{LoadMusicStream(audio_info.path.c_str())};
        music_it = this->music_cache.emplace(audio_info.path, music).first;
    }

    music_it->second.looping = audio_info.loop;

    this->current_sound = music_it->second;

    PlayMusicStream(music_it->second);
}

auto AudioManager::resume() const -> void {
    if (!this->current_sound.has_value()) {
        throw std::runtime_error{"No sound to resume"};
    }

    ResumeMusicStream(this->current_sound.value());
}

auto AudioManager::pause() const -> void {
    if (!this->current_sound.has_value()) {
        throw std::runtime_error{"No sound to pause"};
    }

    PauseMusicStream(this->current_sound.value());
}

auto AudioManager::stop() const -> void {
    if (!this->current_sound.has_value()) {
        throw std::runtime_error{"No sound to stop"};
    }

    StopMusicStream(this->current_sound.value());
}

AudioManager::AudioManager() {
    InitAudioDevice();
}

AudioManager::~AudioManager() {
    for (const auto& [_, music] : this->music_cache) {
        UnloadMusicStream(music);
    }

    CloseAudioDevice();
}

} // namespace engine
