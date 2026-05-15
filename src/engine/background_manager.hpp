#pragma once

#include "../parser/instruction.hpp"
#include "raylib.h"
#include <filesystem>
#include <functional>
#include <optional>
#include <unordered_map>

namespace engine {

class BackgroundManager {
  public:
    BackgroundManager(const BackgroundManager&) = delete;
    BackgroundManager(BackgroundManager&&) = delete;

    auto operator=(const BackgroundManager&) -> BackgroundManager& = delete;
    auto operator=(BackgroundManager&&) -> BackgroundManager& = delete;

    static auto instance() -> BackgroundManager&;

    auto update() -> void;

    auto show(const parser::BackgroundShow& background_info) -> void;
    auto hide() -> void;

  private:
    struct BackgroundTransition {
        std::reference_wrapper<Texture2D> from;
        std::reference_wrapper<Texture2D> to;
        parser::Transition::Value transition;
        double duration;
        double elapsed{0.0};
    };

    BackgroundManager() = default;
    ~BackgroundManager();

    std::unordered_map<std::filesystem::path, Texture2D> background_cache;
    std::optional<std::reference_wrapper<Texture2D>> current_background{
      std::nullopt
    };
    std::optional<BackgroundTransition> current_transition{std::nullopt};
};

} // namespace engine
