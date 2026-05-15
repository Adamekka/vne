#pragma once

#include "../parser/instruction.hpp"
#include "raylib.h"
#include <unordered_map>

namespace engine {

class SpriteManager {
  public:
    SpriteManager(const SpriteManager&) = delete;
    SpriteManager(SpriteManager&&) = delete;

    auto operator=(const SpriteManager&) -> SpriteManager& = delete;
    auto operator=(SpriteManager&&) -> SpriteManager& = delete;

    static auto instance() -> SpriteManager&;

    auto update() -> void;

    auto show(const parser::SpriteShow& sprite_info) -> void;
    auto hide(const parser::SpriteHide& sprite_info) -> void;

  private:
    struct SpriteTransition {
        parser::SpritePosition::Value position;
        std::optional<std::reference_wrapper<Texture2D>> from;
        std::reference_wrapper<Texture2D> to;
        parser::Transition::Value transition;
        double duration;
        double elapsed{0.0};
    };

    SpriteManager() = default;
    ~SpriteManager();

    std::unordered_map<std::filesystem::path, Texture2D> sprite_cache;
    std::unordered_map<
        parser::SpritePosition::Value,
        std::reference_wrapper<Texture2D>>
        current_sprites;
    std::optional<SpriteTransition> current_transition{std::nullopt};
};

} // namespace engine
