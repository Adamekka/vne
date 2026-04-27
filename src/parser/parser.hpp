#pragma once

#include "parsed_script.hpp"

namespace parser {

class Parser {
  public:
    explicit Parser(std::filesystem::path path);

    [[nodiscard]] auto parse() const -> ParsedScript;

  private:
    std::filesystem::path path;
};

} // namespace parser
