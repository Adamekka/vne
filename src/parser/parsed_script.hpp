#pragma once

#include "instruction.hpp"
#include <vector>

namespace parser {

class ParsedScript {
  public:
    ParsedScript(
        std::unique_ptr<const std::string> source,
        std::vector<Instruction::Value> instructions
    );

    ParsedScript(const ParsedScript&) = delete;
    ParsedScript(ParsedScript&&) = default;

    ~ParsedScript() = default;

    auto operator=(const ParsedScript&) -> ParsedScript& = delete;
    auto operator=(ParsedScript&&) -> ParsedScript& = default;

    [[nodiscard]] auto get_instructions() const
        -> const std::vector<Instruction::Value>&;

  private:
    std::unique_ptr<const std::string> source;
    std::vector<Instruction::Value> instructions;
};

} // namespace parser
