#include "parsed_script.hpp"

namespace parser {

ParsedScript::ParsedScript(
    std::unique_ptr<const std::string> source,
    std::vector<Instruction::Value> instructions
)
    : source{std::move(source)}
    , instructions{std::move(instructions)} {}

auto ParsedScript::get_instructions() const
    -> const std::vector<Instruction::Value>& {
    return instructions;
}

} // namespace parser
