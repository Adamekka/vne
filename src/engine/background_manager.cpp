#include "background_manager.hpp"
#include <algorithm>

namespace engine {

auto BackgroundManager::instance() -> BackgroundManager& {
    static BackgroundManager instance;
    return instance;
}

auto BackgroundManager::update() -> void {
    const auto draw_background{
      [](const Texture2D& texture,
         const float x,
         const float y,
         const Color tint) -> void {
          const Rectangle source{
            .x = 0,
            .y = 0,
            .width = static_cast<float>(texture.width),
            .height = static_cast<float>(texture.height)
          };

          const Rectangle dest{
            .x = x,
            .y = y,
            .width = static_cast<float>(GetScreenWidth()),
            .height = static_cast<float>(GetScreenHeight())
          };

          const Vector2 origin{.x = 0, .y = 0};

          DrawTexturePro(texture, source, dest, origin, 0, tint);
      }
    };

    if (this->current_transition.has_value()) {
        BackgroundTransition& transition{this->current_transition.value()};
        transition.elapsed += static_cast<double>(GetFrameTime());

        double progress{transition.elapsed / transition.duration};

        progress = std::min(progress, 1.0);

        const float screen_width{static_cast<float>(GetScreenWidth())};
        const float screen_height{static_cast<float>(GetScreenHeight())};
        const float remaining{static_cast<float>(1.0 - progress)};

        switch (transition.transition) {
            case parser::Transition::Value::Fade: {
                draw_background(transition.from.get(), 0, 0, WHITE);
                draw_background(
                    transition.to.get(),
                    0,
                    0,
                    Fade(WHITE, static_cast<float>(progress))
                );
            } break;

            case parser::Transition::Value::SlideInTop: {
                draw_background(transition.from.get(), 0, 0, WHITE);
                draw_background(
                    transition.to.get(), 0, -screen_height * remaining, WHITE
                );
            } break;

            case parser::Transition::Value::SlideInBottom: {
                draw_background(transition.from.get(), 0, 0, WHITE);
                draw_background(
                    transition.to.get(), 0, screen_height * remaining, WHITE
                );
            } break;

            case parser::Transition::Value::SlideInLeft: {
                draw_background(transition.from.get(), 0, 0, WHITE);
                draw_background(
                    transition.to.get(), -screen_width * remaining, 0, WHITE
                );
            } break;

            case parser::Transition::Value::SlideInRight: {
                draw_background(transition.from.get(), 0, 0, WHITE);
                draw_background(
                    transition.to.get(), screen_width * remaining, 0, WHITE
                );
            } break;
        }

        if (progress >= 1.0) {
            this->current_background = transition.to;
            this->current_transition = std::nullopt;
        }

        return;
    }

    if (!this->current_background.has_value()) {
        return;
    }

    const Texture2D& texture = this->current_background.value().get();

    draw_background(texture, 0, 0, WHITE);
}

auto BackgroundManager::show(const parser::BackgroundShow& background_info)
    -> void {
    auto background_it{this->background_cache.find(background_info.path)};

    if (background_it == this->background_cache.end()) {
        if (!std::filesystem::exists(background_info.path)) {
            throw std::runtime_error{
              "Background file does not exist: " + background_info.path.string()
            };
        }

        auto background{LoadTexture(background_info.path.c_str())};
        background_it
            = this->background_cache.emplace(background_info.path, background)
                  .first;
    }

    if (this->current_transition.has_value()) {
        this->current_background = this->current_transition.value().to;
        this->current_transition = std::nullopt;
    }

    if (!background_info.transition.has_value()
        || !background_info.duration.has_value()
        || background_info.duration.value().value <= 0.0
        || !this->current_background.has_value()) {
        this->current_background = background_it->second;
        return;
    }

    Texture2D& current{this->current_background.value().get()};
    Texture2D& next{background_it->second};

    if (&current == &next) {
        this->current_background = next;
        return;
    }

    this->current_transition.emplace(
        BackgroundTransition{
          .from = std::ref(current),
          .to = std::ref(next),
          .transition = background_info.transition.value().value,
          .duration = background_info.duration.value().value
        }
    );
}

auto BackgroundManager::hide() -> void {
    this->current_background = std::nullopt;
    this->current_transition = std::nullopt;
}

BackgroundManager::~BackgroundManager() {
    for (auto& [_, background] : this->background_cache) {
        UnloadTexture(background);
    }
}

} // namespace engine
