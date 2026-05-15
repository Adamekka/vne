#pragma once

#include "parsed_script.hpp"

namespace parser {

class Parser {
  public:
    explicit Parser(std::filesystem::path path);

    Parser(const Parser&) = delete;
    Parser(Parser&&) = delete;

    ~Parser() = default;

    auto operator=(const Parser&) -> Parser& = delete;
    auto operator=(Parser&&) -> Parser& = delete;

    [[nodiscard]] auto parse() const -> ParsedScript;

  private:
    std::filesystem::path path;
};

} // namespace parser
