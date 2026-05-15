#pragma once

#include "../parser/instruction.hpp"
#include "raylib.h"
#include <unordered_map>

namespace engine {

class AudioManager {
  public:
    AudioManager(const AudioManager&) = delete;
    AudioManager(AudioManager&&) = delete;

    auto operator=(const AudioManager&) -> AudioManager& = delete;
    auto operator=(AudioManager&&) -> AudioManager& = delete;

    static auto instance() -> AudioManager&;

    auto update() const -> void;

    auto play(const parser::AudioPlay& audio_info) -> void;
    auto resume() const -> void;
    auto pause() const -> void;
    auto stop() const -> void;

  private:
    AudioManager();
    ~AudioManager();

    std::unordered_map<std::filesystem::path, Music> music_cache;
    std::optional<std::reference_wrapper<Music>> current_sound{std::nullopt};
};

} // namespace engine
