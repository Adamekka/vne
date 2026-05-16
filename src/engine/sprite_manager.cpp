#include "sprite_manager.hpp"
#include "background_manager.hpp"

namespace engine {

auto SpriteManager::instance() -> SpriteManager& {
    static SpriteManager instance;
    return instance;
}

auto SpriteManager::update() -> void {
    // Sprites are authored for the initial scene height; background texture
    // resolution only affects image quality, not layout scale.
    constexpr float AUTHORED_SCENE_HEIGHT{576.0f};

    const auto draw_sprite{
      [](const Texture2D& texture,
         const parser::SpritePosition::Value position,
         const float offset_x,
         const float offset_y,
         const Color tint) -> void {
          const float screen_width{static_cast<float>(GetScreenWidth())};
          const float screen_height{static_cast<float>(GetScreenHeight())};
          Rectangle scene{
            .x = 0, .y = 0, .width = screen_width, .height = screen_height
          };

          if (const auto background_bounds{
                BackgroundManager::instance().current_background_bounds()
              }) {
              scene = background_bounds.value();
          }

          const float scale{scene.height / AUTHORED_SCENE_HEIGHT};

          const float texture_width{static_cast<float>(texture.width) * scale};
          const float texture_height{
            static_cast<float>(texture.height) * scale
          };

          float x = [position, scene, texture_width]() -> float {
              switch (position) {
                  case parser::SpritePosition::Value::Left: {
                      return scene.x + (scene.width * 0.25f)
                           - (texture_width / 2.0f);
                  } break;

                  case parser::SpritePosition::Value::Center: {
                      return scene.x + ((scene.width - texture_width) / 2.0f);
                  } break;

                  case parser::SpritePosition::Value::Right: {
                      return scene.x + (scene.width * 0.75f)
                           - (texture_width / 2.0f);
                  } break;
              }
          }();

          const Rectangle source{
            .x = 0,
            .y = 0,
            .width = static_cast<float>(texture.width),
            .height = static_cast<float>(texture.height)
          };

          const Rectangle dest{
            .x = x + offset_x,
            .y = scene.y + scene.height - texture_height + offset_y,
            .width = texture_width,
            .height = texture_height
          };

          const Vector2 origin{.x = 0, .y = 0};

          DrawTexturePro(texture, source, dest, origin, 0, tint);
      }
    };

    auto transitioning_sprite{this->current_sprites.end()};
    SpriteTransition* transition{nullptr};
    double progress{1.0};
    float remaining{0};

    if (this->current_transition.has_value()) {
        transition = &this->current_transition.value();

        transition->elapsed += static_cast<double>(GetFrameTime());

        progress = transition->elapsed / transition->duration;

        progress = std::min(progress, 1.0);

        remaining = static_cast<float>(1.0 - progress);

        transitioning_sprite = this->current_sprites.find(transition->position);
    }

    const auto draw_position{
      [this,
       &draw_sprite,
       &transitioning_sprite,
       transition,
       progress,
       remaining](const parser::SpritePosition::Value position) -> void {
          auto sprite_it{this->current_sprites.find(position)};

          if (sprite_it == this->current_sprites.end()) {
              return;
          }

          if (transition == nullptr || sprite_it != transitioning_sprite) {
              draw_sprite(sprite_it->second.get(), position, 0, 0, WHITE);
              return;
          }

          const float screen_width{static_cast<float>(GetScreenWidth())};
          const float screen_height{static_cast<float>(GetScreenHeight())};
          Rectangle scene{
            .x = 0, .y = 0, .width = screen_width, .height = screen_height
          };

          if (const auto background_bounds{
                BackgroundManager::instance().current_background_bounds()
              }) {
              scene = background_bounds.value();
          }
          const auto draw_from{[&draw_sprite, position, transition]() -> void {
              if (!transition->from.has_value()) {
                  return;
              }

              draw_sprite(
                  transition->from.value().get(), position, 0, 0, WHITE
              );
          }};

          switch (transition->transition) {
              case parser::Transition::Value::Fade: {
                  draw_from();
                  draw_sprite(
                      transition->to.get(),
                      position,
                      0,
                      0,
                      Fade(WHITE, static_cast<float>(progress))
                  );
              } break;

              case parser::Transition::Value::SlideInTop: {
                  draw_from();
                  draw_sprite(
                      transition->to.get(),
                      position,
                      0,
                      -scene.height * remaining,
                      WHITE
                  );
              } break;

              case parser::Transition::Value::SlideInBottom: {
                  draw_from();
                  draw_sprite(
                      transition->to.get(),
                      position,
                      0,
                      scene.height * remaining,
                      WHITE
                  );
              } break;

              case parser::Transition::Value::SlideInLeft: {
                  draw_from();
                  draw_sprite(
                      transition->to.get(),
                      position,
                      -scene.width * remaining,
                      0,
                      WHITE
                  );
              } break;

              case parser::Transition::Value::SlideInRight: {
                  draw_from();
                  draw_sprite(
                      transition->to.get(),
                      position,
                      scene.width * remaining,
                      0,
                      WHITE
                  );
              } break;
          }
      }
    };

    draw_position(parser::SpritePosition::Value::Left);
    draw_position(parser::SpritePosition::Value::Center);
    draw_position(parser::SpritePosition::Value::Right);

    if (transition != nullptr && progress >= 1.0) {
        if (transitioning_sprite != this->current_sprites.end()) {
            transitioning_sprite->second = transition->to;
        }

        this->current_transition = std::nullopt;
    }
}

auto SpriteManager::show(const parser::SpriteShow& sprite_info) -> void {
    auto sprite_it{this->sprite_cache.find(sprite_info.path)};

    if (sprite_it == this->sprite_cache.end()) {
        if (!std::filesystem::exists(sprite_info.path)) {
            throw std::runtime_error{
              "Sprite file does not exist: " + sprite_info.path.string()
            };
        }

        auto sprite{LoadTexture(sprite_info.path.c_str())};
        sprite_it = this->sprite_cache.emplace(sprite_info.path, sprite).first;
    }

    if (this->current_transition.has_value()) {
        SpriteTransition& transition{this->current_transition.value()};

        this->current_sprites.insert_or_assign(
            transition.position, transition.to
        );

        this->current_transition = std::nullopt;
    }

    const parser::SpritePosition::Value position{sprite_info.position.value};
    auto current_sprite{this->current_sprites.find(position)};

    if (!sprite_info.transition.has_value() || !sprite_info.duration.has_value()
        || sprite_info.duration.value().value <= 0.0) {
        this->current_sprites.insert_or_assign(
            position, std::ref(sprite_it->second)
        );
        return;
    }

    Texture2D& next{sprite_it->second};

    if (current_sprite == this->current_sprites.end()) {
        this->current_sprites.insert_or_assign(position, std::ref(next));
        this->current_transition.emplace(
            SpriteTransition{
              .position = position,
              .from = std::nullopt,
              .to = std::ref(next),
              .transition = sprite_info.transition.value().value,
              .duration = sprite_info.duration.value().value
            }
        );
        return;
    }

    Texture2D& current{current_sprite->second.get()};

    if (&current == &next) {
        current_sprite->second = std::ref(next);
        return;
    }

    this->current_transition.emplace(
        SpriteTransition{
          .position = position,
          .from = std::ref(current),
          .to = std::ref(next),
          .transition = sprite_info.transition.value().value,
          .duration = sprite_info.duration.value().value
        }
    );
}

auto SpriteManager::hide(const parser::SpriteHide& sprite_info) -> void {
    this->current_sprites.erase(sprite_info.position.value);
    this->current_transition = std::nullopt;
}

SpriteManager::~SpriteManager() {
    for (auto& [_, sprite] : this->sprite_cache) {
        UnloadTexture(sprite);
    }
}

} // namespace engine
