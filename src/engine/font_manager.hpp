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
    FontManager();
    ~FontManager() = default;

    Font font;
};

} // namespace engine
