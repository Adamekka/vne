#include "font_manager.hpp"

namespace engine {

auto FontManager::instance() -> FontManager& {
    static FontManager instance;
    return instance;
}

[[nodiscard]] auto FontManager::get_font() const -> const Font& {
    return this->font;
}

FontManager::FontManager()
    : font(LoadFont("font.ttf")) {}

} // namespace engine
