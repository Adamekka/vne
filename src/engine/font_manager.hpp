#pragma once

#include "raylib.h"

namespace engine {

class FontManager {
  public:
    FontManager(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;

    auto operator=(const FontManager&) -> FontManager& = delete;
    auto operator=(FontManager&&) -> FontManager& = delete;

    static auto instance() -> FontManager&;

    [[nodiscard]] auto get_font() const -> const Font&;

  private:
    FontManager() = default;
    ~FontManager();

    Font font{LoadFont("font.ttf")};
};

} // namespace engine
