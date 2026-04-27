#pragma once

#include "../engine/variable.hpp"
#include "parseable.hpp"
#include <filesystem>
#include <optional>
#include <variant>

namespace parser {

// MARK: - Shared Types

struct Duration {
    double value;

    explicit Duration(double value);

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<Duration>;
};

struct Transition {
    enum class Value : uint8_t {
        Fade,
        SlideInTop,
        SlideInBottom,
        SlideInLeft,
        SlideInRight,
    } value;

    // NOLINTNEXTLINE(google-explicit-constructor)
    Transition(Value value);

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<Transition>;
};

using LabelValue = std::string_view;

struct Target {
    using File = std::filesystem::path;
    using Value = std::variant<LabelValue, File>;

    Value target;

    template<typename T>
    constexpr explicit Target(T target): target{std::move(target)} {}

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<Target>;
};

using VariableName = std::string_view;
using EngineInt = engine::Variable::Type;

// MARK: - Version

struct Version {
    enum class Value : uint8_t {
        V1,
    } value;

    // NOLINTNEXTLINE(google-explicit-constructor)
    Version(Value value);

    static auto parse(std::string_view input) -> ParseResult<Version>;
};

// MARK: - Label

struct Label {
    LabelValue value;

    explicit Label(LabelValue value);

    static auto parse(std::string_view input) -> ParseResult<Label>;
};

// MARK: -  Audio

struct AudioPlay {
    std::filesystem::path path;
    bool loop;

    AudioPlay(std::filesystem::path path, bool loop);

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<AudioPlay>;
};

struct AudioResume {};

struct AudioPause {};

struct AudioStop {};

struct Audio {
    using Action = std::variant<AudioPlay, AudioResume, AudioPause, AudioStop>;
    Action action;

    template<typename T>
    constexpr explicit Audio(T action): action{std::move(action)} {}

    static auto parse(std::string_view input) -> ParseResult<Audio>;
};

// MARK: - Background

struct BackgroundShow {
    std::filesystem::path path;
    std::optional<Transition> transition;
    std::optional<Duration> duration;

    explicit BackgroundShow(
        std::filesystem::path path,
        std::optional<Transition> transition,
        std::optional<Duration> duration
    );

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<BackgroundShow>;
};

struct BackgroundHide {};

struct Background {
    using Value = std::variant<BackgroundShow, BackgroundHide>;
    Value value;

    template<typename T>
    constexpr explicit Background(T value): value{std::move(value)} {}

    static auto parse(std::string_view input) -> ParseResult<Background>;
};

// MARK: - VariableMutation

struct VariableMutation {
    enum class Operation : uint8_t {
        Set,
        Add,
    } op;

    VariableName variable_name;
    EngineInt value;

    VariableMutation(Operation op, VariableName variable_name, EngineInt value);

    static auto parse(std::string_view input) -> ParseResult<VariableMutation>;
};

// MARK: - If

struct Atom {
    std::variant<VariableName, EngineInt> value;

    template<typename T>
    constexpr explicit Atom(T value): value{std::move(value)} {}

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<Atom>;
};

struct Operator {
    enum class Value : uint8_t {
        Equals,
        NotEquals,
        GreaterThan,
        LessThan,
        GreaterThanOrEqual,
        LessThanOrEqual,
    } value;

    // NOLINTNEXTLINE(google-explicit-constructor)
    Operator(Value value);

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<Operator>;
};

struct If {
    Atom lhs;
    Operator op;
    Atom rhs;

    If(Atom lhs, Operator op, Atom rhs);

    static auto parse(std::string_view input) -> ParseResult<If>;
};

struct EndIf {
    static auto parse(std::string_view input) -> ParseResult<EndIf>;
};

// MARK: - Jump

struct Jump {
    Target target;

    explicit Jump(Target target);

    static auto parse(std::string_view input) -> ParseResult<Jump>;
};

// MARK: - Sprite

struct SpritePosition {
    enum class Value : uint8_t {
        Left,
        Center,
        Right,
    } value;

    explicit SpritePosition(Value value);

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<SpritePosition>;
};

struct SpriteShow {
    SpritePosition position;
    std::filesystem::path path;
    std::optional<Transition> transition;
    std::optional<Duration> duration;

    explicit SpriteShow(
        SpritePosition position,
        std::filesystem::path path,
        std::optional<Transition> transition,
        std::optional<Duration> duration
    );

    static auto parse(std::string_view input, std::string_view original_input)
        -> ParseResult<SpriteShow>;
};

struct SpriteHide {
    SpritePosition position;

    explicit SpriteHide(SpritePosition position);
};

struct Sprite {
    std::variant<SpriteShow, SpriteHide> value;

    template<typename T>
    constexpr explicit Sprite(T value): value{std::move(value)} {}

    static auto parse(std::string_view input) -> ParseResult<Sprite>;
};

// MARK: - Wait

struct Wait {
    Duration duration;

    explicit Wait(Duration duration);

    static auto parse(std::string_view input) -> ParseResult<Wait>;
};

// MARK: - Choice

struct Choice {
    std::string_view prompt;
    Target target;

    Choice(std::string_view prompt, Target target);

    static auto parse(std::string_view input) -> ParseResult<Choice>;
};

// MARK: - Dialogue

struct Dialogue {
    std::string_view speaker;
    std::string_view text;

    Dialogue(std::string_view speaker, std::string_view text);

    static auto parse(std::string_view input) -> ParseResult<Dialogue>;
};

// MARK: - Narration

struct Narration {
    std::string_view value;

    explicit Narration(std::string_view value);

    static auto parse(std::string_view input) -> ParseResult<Narration>;
};

// MARK: - Instruction

struct Instruction {
    using Value = std::variant<
        Version,
        Label,
        Audio,
        Background,
        VariableMutation,
        If,
        EndIf,
        Jump,
        Sprite,
        Wait,
        Choice,
        Dialogue,
        Narration>;

    Value value;

    template<typename T>
    constexpr explicit Instruction(T value): value{std::move(value)} {}
};

// MARK: - ExhaustiveParser

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
