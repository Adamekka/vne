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

    [[nodiscard]] auto get_instructions() const
        -> const std::vector<Instruction::Value>&;

  private:
    std::unique_ptr<const std::string> source;
    std::vector<Instruction::Value> instructions;
};

} // namespace parser
