#pragma once

#include "../parser/parsed_script.hpp"
#include <unordered_map>

namespace engine {

// To avoid circular dependency
class Engine;

class Interpreter {
  public:
    explicit Interpreter(Engine& engine, parser::ParsedScript script);

    Interpreter(const Interpreter&) = delete;
    Interpreter(Interpreter&&) = delete;

    ~Interpreter() = default;

    auto operator=(const Interpreter&) -> Interpreter& = delete;
    auto operator=(Interpreter&&) -> Interpreter& = delete;

    auto new_script(parser::ParsedScript script) -> void;

    auto next() -> void;

    auto select_choice(const parser::Choice& choice) -> void;

  private:
    Engine& engine;

    parser::ParsedScript script;

    size_t instruction_index = 0;

    /// Labels in this script
    /// Key - label name, value - instruction index
    std::unordered_map<std::string, size_t> labels;

    /// Used for If conditions
    std::vector<bool> skipping_stack;

    bool choice_block{false};

    auto prepare_script() -> void;

    auto go_to_target(const parser::Target& target) -> void;

    [[nodiscard]] auto interpolate_text(std::string_view text) const
        -> std::string;
};

} // namespace engine
