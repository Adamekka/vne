#pragma once

#include "parseable.hpp"
#include <variant>

namespace parser {

template<typename Variant> struct ExhaustiveParser;

template<typename... Alts> struct ExhaustiveParser<std::variant<Alts...>> {
    // Compile error here if any Alt lacks a conforming parse() function.
    static_assert(
        (Parseable<Alts> && ...),
        "Every alternative of Instruction::value must define "
        "static auto T::parse(std::string_view) -> ParseResult<T>"
    );

    static auto run(std::string_view in) -> ParseResult<std::variant<Alts...>> {
        ParseResult<std::variant<Alts...>> result{
          std::unexpected{ParseError{ParseError::Kind::UnknownPrefix, in}}
        };

        // Try each alternative in declaration order. First success wins.
        // Fold-expression short-circuits via `||`.
        auto try_one = [&]<typename T>() -> bool {
            if (result) {
                return true;
            }

            auto r{T::parse(in)};

            if (r) {
                result = std::variant<Alts...>{std::move(*r)};
                return true;
            }

            // Propagate "real" errors; swallow "prefix didn't match" so the
            // next alternative can have a go.
            if (r.error().kind != ParseError::Kind::UnknownPrefix) {
                result = std::unexpected{std::move(r).error()};
                return true; // stop - we found the intended alternative but it
                             // was malformed
            }
            return false;
        };

        (try_one.template operator()<Alts>() || ...);
        return result;
    }
};

} // namespace parser
