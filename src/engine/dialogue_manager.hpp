#pragma once

#include <string>

namespace engine {

class DialogueManager {
  public:
    DialogueManager(const DialogueManager&) = delete;
    DialogueManager(DialogueManager&&) = delete;

    auto operator=(const DialogueManager&) -> DialogueManager& = delete;
    auto operator=(DialogueManager&&) -> DialogueManager& = delete;

    static auto instance() -> DialogueManager&;

    auto update() -> void;

    auto show_dialogue(std::string speaker, std::string text) -> void;

    auto show_narration(std::string text) -> void;

  private:
    enum class LineKind : uint8_t {
        Dialogue,
        Narration,
    };

    DialogueManager() = default;
    ~DialogueManager() = default;

    LineKind line_kind{LineKind::Narration};
    std::string speaker;
    std::string text;
};

} // namespace engine
