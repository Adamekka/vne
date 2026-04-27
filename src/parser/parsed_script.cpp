#include "parsed_script.hpp"

namespace parser {

ParsedScript::ParsedScript(
    std::unique_ptr<const std::string> source,
    std::vector<Instruction::Value> instructions
): source{std::move(source)}, instructions{std::move(instructions)} {}

} // namespace parser
