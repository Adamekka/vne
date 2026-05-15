#pragma once

#include "../parser/instruction.hpp"
#include <vector>

namespace engine {

class ChoiceManager {
  public:
    ChoiceManager(const ChoiceManager&) = delete;
    ChoiceManager(ChoiceManager&&) = delete;

    auto operator=(const ChoiceManager&) -> ChoiceManager& = delete;
    auto operator=(ChoiceManager&&) -> ChoiceManager& = delete;

    static auto instance() -> ChoiceManager&;

    auto update() -> std::optional<parser::Choice>;

    auto show(const parser::Choice& choice) -> void;

    auto clear() -> void;

    [[nodiscard]] auto get_choices() const
        -> const std::vector<parser::Choice>&;

  private:
    ChoiceManager() = default;
    ~ChoiceManager() = default;

    std::vector<parser::Choice> choices;
};

} // namespace engine
