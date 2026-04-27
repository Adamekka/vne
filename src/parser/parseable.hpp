#pragma once

#include "parse_result.hpp"

namespace parser {

template<typename T>
concept Parseable = requires(std::string_view sv) {
    { T::parse(sv) } -> std::same_as<ParseResult<T>>;
};

} // namespace parser
