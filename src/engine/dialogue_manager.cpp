#include "dialogue_manager.hpp"
#include "background_manager.hpp"
#include "font_manager.hpp"
#include <algorithm>
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
    // UI constants are authored for the initial scene height; high-resolution
    // background textures should not shrink the interface.
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
    const float speaker_font_size{
      static_cast<float>(SPEAKER_FONT_SIZE) * scale
    };
    const float spacing{static_cast<float>(SPACING) * scale};
    const float horizontal_padding{
      static_cast<float>(HORIZONTAL_PADDING) * scale
    };
    const float vertical_padding{static_cast<float>(VERTICAL_PADDING) * scale};
    const float panel_margin{static_cast<float>(PANEL_MARGIN) * scale};
    const float panel_bottom_margin{
      static_cast<float>(PANEL_BOTTOM_MARGIN) * scale
    };
    const float panel_height{static_cast<float>(PANEL_HEIGHT) * scale};
    const float line_gap{static_cast<float>(LINE_GAP) * scale};
    const float speaker_height{static_cast<float>(SPEAKER_HEIGHT) * scale};
    const float panel_width{
      std::max(scene.width - (panel_margin * 2.0f), horizontal_padding * 2.0f)
    };
    const float panel_x{scene.x + ((scene.width - panel_width) / 2.0f)};
    const float panel_y{
      scene.y
      + std::max(
          scene.height - panel_height - panel_bottom_margin, panel_margin
      )
    };
    const Font& font{FontManager::instance().get_font()};
    const Rectangle panel{
      .x = panel_x, .y = panel_y, .width = panel_width, .height = panel_height
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
    DrawRectangleRoundedLinesEx(
        panel, 0.16f, 16, 2.0f * scale, Fade(WHITE, 0.75f)
    );

    if (this->line_kind == LineKind::Dialogue) {
        const Vector2 speaker_size{
          MeasureTextEx(font, this->speaker.c_str(), speaker_font_size, spacing)
        };
        const Rectangle speaker_panel{
          .x = panel.x + horizontal_padding,
          .y = panel.y - (speaker_height / 2.0f),
          .width = speaker_size.x + horizontal_padding,
          .height = speaker_height
        };
        const Vector2 speaker_position{
          .x = speaker_panel.x + (horizontal_padding / 2.0f),
          .y = speaker_panel.y + ((speaker_height - speaker_size.y) / 2.0f)
        };

        DrawRectangleRounded(speaker_panel, 0.35f, 12, Fade(DARKBLUE, 0.86f));
        DrawRectangleRoundedLinesEx(
            speaker_panel, 0.35f, 12, 2.0f * scale, WHITE
        );
        DrawTextEx(
            font,
            this->speaker.c_str(),
            speaker_position,
            speaker_font_size,
            spacing,
            WHITE
        );
    }

    const float text_width{panel_width - (horizontal_padding * 2.0f)};
    const std::vector<std::string> lines{
      wrap_text(this->text, text_width, font_size, spacing)
    };
    float text_y{panel.y + vertical_padding};

    for (const std::string& line : lines) {
        const Vector2 text_position{
          .x = panel.x + horizontal_padding, .y = text_y
        };

        DrawTextEx(
            font, line.c_str(), text_position, font_size, spacing, WHITE
        );
        text_y += font_size + line_gap;
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
