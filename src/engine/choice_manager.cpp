#include "choice_manager.hpp"
#include "background_manager.hpp"
#include "font_manager.hpp"

namespace engine {

auto ChoiceManager::instance() -> ChoiceManager& {
    static ChoiceManager instance;
    return instance;
}

auto ChoiceManager::update() -> std::optional<parser::Choice> {
    if (this->choices.empty()) {
        return std::nullopt;
    }

    constexpr int32_t FONT_SIZE{32};
    constexpr int32_t SPACING{1};
    constexpr int32_t HORIZONTAL_PADDING{32};
    constexpr int32_t BUTTON_HEIGHT{64};
    constexpr int32_t BUTTON_SPACING{16};
    constexpr int32_t MIN_BUTTON_WIDTH{360};
    // Choice UI is authored for the initial scene height, independent of the
    // background texture's pixel density.
    constexpr float AUTHORED_SCENE_HEIGHT{576.0f};

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
    const float font_size{static_cast<float>(FONT_SIZE) * scale};
    const float spacing{static_cast<float>(SPACING) * scale};
    const float horizontal_padding{
      static_cast<float>(HORIZONTAL_PADDING) * scale
    };
    const float button_height{static_cast<float>(BUTTON_HEIGHT) * scale};
    const float button_spacing{static_cast<float>(BUTTON_SPACING) * scale};
    const Font& font{FontManager::instance().get_font()};
    float button_width{static_cast<float>(MIN_BUTTON_WIDTH) * scale};

    for (const parser::Choice& choice : this->choices) {
        const std::string prompt{choice.prompt};
        const Vector2 text_size{
          MeasureTextEx(font, prompt.c_str(), font_size, spacing)
        };
        const float needed_width{text_size.x + (horizontal_padding * 2.0f)};

        button_width = std::max(needed_width, button_width);
    }

    const float max_button_width{scene.width - (horizontal_padding * 2.0f)};

    button_width = std::min(button_width, max_button_width);

    const float total_height{
      (static_cast<float>(this->choices.size()) * button_height)
      + (static_cast<float>(this->choices.size() - 1) * button_spacing)
    };
    const float button_x{scene.x + ((scene.width - button_width) / 2.0f)};
    const float start_y{scene.y + ((scene.height - total_height) / 2.0f)};
    const Vector2 mouse_position{GetMousePosition()};
    const bool mouse_pressed{IsMouseButtonPressed(MOUSE_LEFT_BUTTON)};
    std::optional<parser::Choice> selected_choice{std::nullopt};

    for (size_t i{0}; i < this->choices.size(); i++) {
        const parser::Choice& choice{this->choices.at(i)};
        const std::string prompt{choice.prompt};
        const Vector2 text_size{
          MeasureTextEx(font, prompt.c_str(), font_size, spacing)
        };
        const float button_y{
          start_y + (static_cast<float>(i) * (button_height + button_spacing))
        };
        const Rectangle button{
          .x = button_x,
          .y = button_y,
          .width = button_width,
          .height = button_height
        };
        const bool is_hovered{CheckCollisionPointRec(mouse_position, button)};
        const Color fill{
          is_hovered ? Fade(DARKBLUE, 0.86f) : Fade(BLACK, 0.72f)
        };
        const Color border{is_hovered ? WHITE : Fade(WHITE, 0.75f)};

        if (is_hovered && mouse_pressed) {
            selected_choice = choice;
        }

        DrawRectangleRounded(button, 0.25f, 12, fill);
        DrawRectangleRoundedLinesEx(button, 0.25f, 12, 2.0f * scale, border);

        const Vector2 position{
          .x = button.x + ((button.width - text_size.x) / 2.0f),
          .y = button.y + ((button.height - text_size.y) / 2.0f)
        };

        DrawTextEx(font, prompt.c_str(), position, font_size, spacing, WHITE);
    }

    return selected_choice;
}

auto ChoiceManager::show(const parser::Choice& choice) -> void {
    this->choices.emplace_back(choice);
}

auto ChoiceManager::clear() -> void {
    this->choices.clear();
}

[[nodiscard]] auto ChoiceManager::get_choices() const
    -> const std::vector<parser::Choice>& {
    return this->choices;
}

} // namespace engine
