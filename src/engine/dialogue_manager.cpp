#include "dialogue_manager.hpp"
#include "font_manager.hpp"
#include <sstream>
#include <vector>

namespace engine {

auto DialogueManager::instance() -> DialogueManager& {
    static DialogueManager instance;
    return instance;
}

auto DialogueManager::update() -> void {
    constexpr int32_t FONT_SIZE{28};
    constexpr int32_t SPEAKER_FONT_SIZE{24};
    constexpr int32_t SPACING{1};
    constexpr int32_t HORIZONTAL_PADDING{28};
    constexpr int32_t VERTICAL_PADDING{28};
    constexpr int32_t PANEL_MARGIN{48};
    constexpr int32_t PANEL_BOTTOM_MARGIN{36};
    constexpr int32_t PANEL_HEIGHT{170};
    constexpr int32_t LINE_GAP{8};
    constexpr int32_t SPEAKER_HEIGHT{42};

    const int32_t screen_width{static_cast<int32_t>(GetScreenWidth())};
    const int32_t screen_height{static_cast<int32_t>(GetScreenHeight())};
    const int32_t panel_width{
      std::max(screen_width - (PANEL_MARGIN * 2), HORIZONTAL_PADDING * 2)
    };
    const int32_t panel_x{(screen_width - panel_width) / 2};
    const int32_t panel_y{
      std::max(screen_height - PANEL_HEIGHT - PANEL_BOTTOM_MARGIN, PANEL_MARGIN)
    };
    const Font& font{FontManager::instance().get_font()};
    const Rectangle panel{
      .x = static_cast<float>(panel_x),
      .y = static_cast<float>(panel_y),
      .width = static_cast<float>(panel_width),
      .height = static_cast<float>(PANEL_HEIGHT)
    };

    const auto wrap_text{
      [&font](
          const std::string& value,
          const float max_width,
          const float font_size,
          const float spacing
      ) -> std::vector<std::string> {
          std::vector<std::string> lines;
          std::istringstream words{value};
          std::string line;
          std::string word;

          while (words >> word) {
              std::string candidate{line};

              if (!candidate.empty()) {
                  candidate += ' ';
              }

              candidate += word;

              const Vector2 candidate_size{
                MeasureTextEx(font, candidate.c_str(), font_size, spacing)
              };

              if (candidate_size.x <= max_width || line.empty()) {
                  line = candidate;
                  continue;
              }

              lines.emplace_back(line);
              line = word;
          }

          if (!line.empty()) {
              lines.emplace_back(line);
          }

          return lines;
      }
    };

    DrawRectangleRounded(panel, 0.16f, 16, Fade(BLACK, 0.72f));
    DrawRectangleRoundedLinesEx(panel, 0.16f, 16, 2.0f, Fade(WHITE, 0.75f));

    if (this->line_kind == LineKind::Dialogue) {
        const Vector2 speaker_size{
          MeasureTextEx(font, this->speaker.c_str(), SPEAKER_FONT_SIZE, SPACING)
        };
        const Rectangle speaker_panel{
          .x = panel.x + static_cast<float>(HORIZONTAL_PADDING),
          .y = panel.y - (static_cast<float>(SPEAKER_HEIGHT) / 2.0f),
          .width = speaker_size.x + static_cast<float>(HORIZONTAL_PADDING),
          .height = static_cast<float>(SPEAKER_HEIGHT)
        };
        const Vector2 speaker_position{
          .x
          = speaker_panel.x + (static_cast<float>(HORIZONTAL_PADDING) / 2.0f),
          .y = speaker_panel.y
             + ((static_cast<float>(SPEAKER_HEIGHT) - speaker_size.y) / 2.0f)
        };

        DrawRectangleRounded(speaker_panel, 0.35f, 12, Fade(DARKBLUE, 0.86f));
        DrawRectangleRoundedLinesEx(speaker_panel, 0.35f, 12, 2.0f, WHITE);
        DrawTextEx(
            font,
            this->speaker.c_str(),
            speaker_position,
            SPEAKER_FONT_SIZE,
            SPACING,
            WHITE
        );
    }

    const float text_width{
      static_cast<float>(panel_width - (HORIZONTAL_PADDING * 2))
    };
    const std::vector<std::string> lines{
      wrap_text(this->text, text_width, FONT_SIZE, SPACING)
    };
    float text_y{panel.y + static_cast<float>(VERTICAL_PADDING)};

    for (const std::string& line : lines) {
        const Vector2 text_position{
          .x = panel.x + static_cast<float>(HORIZONTAL_PADDING), .y = text_y
        };

        DrawTextEx(
            font, line.c_str(), text_position, FONT_SIZE, SPACING, WHITE
        );
        text_y += static_cast<float>(FONT_SIZE + LINE_GAP);
    }
}

auto DialogueManager::show_dialogue(std::string speaker, std::string text)
    -> void {
    this->line_kind = LineKind::Dialogue;
    this->speaker = std::move(speaker);
    this->text = std::move(text);
}

auto DialogueManager::show_narration(std::string text) -> void {
    this->line_kind = LineKind::Narration;
    this->speaker.clear();
    this->text = std::move(text);
}

} // namespace engine
