#include "choice_manager.hpp"
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

    const int32_t screen_width{static_cast<int32_t>(GetScreenWidth())};
    const int32_t screen_height{static_cast<int32_t>(GetScreenHeight())};
    const Font& font{FontManager::instance().get_font()};
    int32_t button_width{MIN_BUTTON_WIDTH};

    for (const parser::Choice& choice : this->choices) {
        const std::string prompt{choice.prompt};
        const Vector2 text_size{
          MeasureTextEx(font, prompt.c_str(), FONT_SIZE, SPACING)
        };
        const int32_t needed_width{
          static_cast<int32_t>(text_size.x) + (HORIZONTAL_PADDING * 2)
        };

        button_width = std::max(needed_width, button_width);
    }

    const int32_t max_button_width{screen_width - (HORIZONTAL_PADDING * 2)};

    button_width = std::min(button_width, max_button_width);

    const int32_t total_height{
      (static_cast<int32_t>(this->choices.size()) * BUTTON_HEIGHT)
      + (static_cast<int32_t>(this->choices.size() - 1) * BUTTON_SPACING)
    };
    const int32_t button_x{(screen_width - button_width) / 2};
    const int32_t start_y{(screen_height - total_height) / 2};
    const Vector2 mouse_position{GetMousePosition()};
    const bool mouse_pressed{IsMouseButtonPressed(MOUSE_LEFT_BUTTON)};
    std::optional<parser::Choice> selected_choice{std::nullopt};

    for (size_t i{0}; i < this->choices.size(); i++) {
        const parser::Choice& choice{this->choices.at(i)};
        const std::string prompt{choice.prompt};
        const Vector2 text_size{
          MeasureTextEx(font, prompt.c_str(), FONT_SIZE, SPACING)
        };
        const int32_t button_y{
          start_y + (static_cast<int32_t>(i) * (BUTTON_HEIGHT + BUTTON_SPACING))
        };
        const Rectangle button{
          .x = static_cast<float>(button_x),
          .y = static_cast<float>(button_y),
          .width = static_cast<float>(button_width),
          .height = static_cast<float>(BUTTON_HEIGHT)
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
        DrawRectangleRoundedLinesEx(button, 0.25f, 12, 2.0f, border);

        const Vector2 position{
          .x
          = static_cast<float>(screen_width - static_cast<int32_t>(text_size.x))
          / 2.0f,
          .y = static_cast<float>(button_y)
             + ((static_cast<float>(BUTTON_HEIGHT) - text_size.y) / 2.0f)
        };

        DrawTextEx(font, prompt.c_str(), position, FONT_SIZE, SPACING, WHITE);
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
