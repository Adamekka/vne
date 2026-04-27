#include "instruction.hpp"
#include <cassert>
#include <charconv>
#include <print>

namespace parser {

// MARK: - Helpers

namespace {

constexpr std::string_view WHITESPACE{" \t\n\r\f\v"};

constexpr auto trim_leading_whitespace(std::string_view& sv) -> void {
    const auto pos{sv.find_first_not_of(WHITESPACE)};
    sv.remove_prefix(pos == std::string_view::npos ? sv.size() : pos);
}

constexpr auto trim_trailing_whitespace(std::string_view& sv) -> void {
    const auto pos{sv.find_last_not_of(WHITESPACE)};
    if (pos != std::string_view::npos) {
        sv.remove_suffix(sv.size() - pos - 1);
    } else {
        sv.remove_suffix(sv.size());
    }
}

constexpr auto trim_all_whitespace(std::string_view& sv) -> void {
    trim_leading_whitespace(sv);
    trim_trailing_whitespace(sv);
}

[[nodiscard]] constexpr auto next_token(std::string_view& sv)
    -> std::string_view {
    trim_leading_whitespace(sv);
    const auto tok{sv.substr(0, sv.find_first_of(WHITESPACE))};
    sv.remove_prefix(tok.size());
    return tok;
}

constexpr auto normalize_transition_args(
    std::optional<Transition>& transition, std::optional<Duration>& duration
) -> void {
    if (transition.has_value() == duration.has_value()) {
        return;
    }

    // TODO: Show error on screen

    if (duration.has_value()) {
        std::println("Duration cannot be specified without a transition");
        duration.reset();
        return;
    }

    std::println(
        "Transition specified without duration. Duration is required when a "
        "transition is specified."
    );

    transition.reset();
}

} // namespace

// MARK: - Duration

Duration::Duration(double value): value{value} {}

auto Duration::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<Duration> {
    if (!input.empty() && input.back() == 's') {
        input.remove_suffix(1);
    }

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::DurationMissingNumber, original_input}
        };
    }

    double value{0};

    auto [ptr, ec]{
      std::from_chars(input.data(), input.data() + input.size(), value)
    };

    if (ec == std::errc{} && ptr == input.data() + input.size() && value >= 0) {
        return Duration{value};
    }

    return std::unexpected{
      ParseError{ParseError::Kind::DurationMalformedNumber, original_input}
    };
}

// MARK: - Transition

Transition::Transition(Value value): value{value} {}

auto Transition::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<Transition> {
    if (input == "fade") {
        return Transition{Transition::Value::Fade};
    }

    if (input == "slide_in_top") {
        return Transition{Transition::Value::SlideInTop};
    }

    if (input == "slide_in_bottom") {
        return Transition{Transition::Value::SlideInBottom};
    }

    if (input == "slide_in_left") {
        return Transition{Transition::Value::SlideInLeft};
    }

    if (input == "slide_in_right") {
        return Transition{Transition::Value::SlideInRight};
    }

    return std::unexpected{
      ParseError{ParseError::Kind::TransitionMalformed, original_input}
    };
}

// MARK: - Target

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto Target::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<Target> {
    trim_leading_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::TargetMalformed, original_input}
        };
    }

    if (input.starts_with('#')) {
        input.remove_prefix(1);

        trim_all_whitespace(input);

        if (input.empty()) {
            return std::unexpected{
              ParseError{ParseError::Kind::TargetMalformed, original_input}
            };
        }

        for (char i : input) {
            if (i == ' ' || i == '\t') {
                return std::unexpected{
                  ParseError{ParseError::Kind::TargetWithSpace, original_input}
                };
            }
        }

        for (char i : input) {
            if ((i < 'a' || i > 'z') && (i < 'A' || i > 'Z') && i != '-'
                && i != '_' && (i < '0' || i > '9')) {
                return std::unexpected{
                  ParseError{ParseError::Kind::TargetMalformed, original_input}
                };
            }
        }

        return Target{LabelValue{input}};
    }

    trim_all_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::TargetMalformed, original_input}
        };
    }

    for (char i : input) {
        if (i == ' ' || i == '\t') {
            return std::unexpected{
              ParseError{ParseError::Kind::TargetWithSpace, original_input}
            };
        }
    }

    for (char i : input) {
        if ((i < 'a' || i > 'z') && (i < 'A' || i > 'Z') && i != '-' && i != '_'
            && i != '.' && i != '/' && (i < '0' || i > '9')) {
            return std::unexpected{
              ParseError{ParseError::Kind::TargetMalformed, original_input}
            };
        }
    }

    if (!input.ends_with(".md")) {
        return std::unexpected{
          ParseError{ParseError::Kind::TargetMalformed, original_input}
        };
    }

    std::filesystem::path path{input};

    return Target{std::move(path)};
}

// MARK: - Version

Version::Version(Value value): value{value} {}

auto Version::parse(std::string_view input) -> ParseResult<Version> {
    if (input != "<!-- v1 -->") {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, input}
        };
    }

    return Version{Version::Value::V1};
}

// MARK: - Label

Label::Label(LabelValue value): value{value} {}

auto Label::parse(LabelValue input) -> ParseResult<Label> {
    const std::string_view original_input{input};

    if (!input.starts_with("#")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, input}
        };
    }

    while (input.starts_with("#")) {
        input.remove_prefix(1);
    }

    trim_all_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::LabelEmpty, original_input}
        };
    }

    for (char i : input) {
        if (i == ' ' || i == '\t') {
            return std::unexpected{
              ParseError{ParseError::Kind::LabelWithSpace, original_input}
            };
        }
    }

    for (char i : input) {
        if ((i < 'a' || i > 'z') && (i < 'A' || i > 'Z') && i != '-' && i != '_'
            && (i < '0' || i > '9')) {
            return std::unexpected{
              ParseError{ParseError::Kind::LabelMalformed, original_input}
            };
        }
    }

    return Label{input};
}

// MARK: - Audio

AudioPlay::AudioPlay(std::filesystem::path path, bool loop):
    path{std::move(path)}, loop{loop} {}

auto AudioPlay::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<AudioPlay> {
    trim_leading_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::AudioMissingPath, original_input}
        };
    }

    const auto path{next_token(input)};

    trim_all_whitespace(input);

    if (input == "loop") {
        return AudioPlay{std::filesystem::path{path}, true};
    }

    if (input.empty()) {
        return AudioPlay{std::filesystem::path{path}, false};
    }

    return std::unexpected{
      ParseError{ParseError::Kind::AudioMalformedLoopFlag, original_input}
    };
}

auto Audio::parse(std::string_view input) -> ParseResult<Audio> {
    const auto original_input{input};

    if (!input.starts_with("> audio")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"> audio"}.size());

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    trim_leading_whitespace(input);

    const auto subcommand{next_token(input)};

    if (subcommand == "play") {
        return AudioPlay::parse(input, original_input)
            .transform([](AudioPlay play) -> Audio {
                return Audio{std::move(play)};
            });
    }

    if (subcommand == "resume") {
        trim_all_whitespace(input);

        if (input.empty()) {
            return Audio{AudioResume{}};
        }

        return std::unexpected{
          ParseError{ParseError::Kind::AudioMalformedResume, original_input}
        };
    }

    if (subcommand == "pause") {
        trim_all_whitespace(input);

        if (input.empty()) {
            return Audio{AudioPause{}};
        }

        return std::unexpected{
          ParseError{ParseError::Kind::AudioMalformedPause, original_input}
        };
    }

    if (subcommand == "stop") {
        trim_all_whitespace(input);

        if (input.empty()) {
            return Audio{AudioStop{}};
        }

        return std::unexpected{
          ParseError{ParseError::Kind::AudioMalformedStop, original_input}
        };
    }

    if (subcommand.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::AudioMissingCommand, original_input}
        };
    }

    return std::unexpected{
      ParseError{ParseError::Kind::AudioUnknownCommand, original_input}
    };
}

// MARK: - Background

BackgroundShow::BackgroundShow(
    std::filesystem::path path,
    std::optional<Transition> transition,
    std::optional<Duration> duration
): path{std::move(path)}, transition{transition}, duration{duration} {
    normalize_transition_args(this->transition, this->duration);
}

auto BackgroundShow::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<BackgroundShow> {
    trim_leading_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::BackgroundMissingPath, original_input}
        };
    }

    const auto path{next_token(input)};

    trim_leading_whitespace(input);

    std::optional<Transition> transition;
    std::optional<Duration> duration;

    if (!input.empty()) {
        const auto transition_str{next_token(input)};

        auto result{Transition::parse(transition_str, original_input)};

        if (!result) {
            return std::unexpected{std::move(result).error()};
        }

        transition = *std::move(result);

        trim_leading_whitespace(input);

        if (!input.empty()) {
            const auto duration_str{next_token(input)};

            auto result{Duration::parse(duration_str, original_input)};

            if (!result) {
                return std::unexpected{std::move(result).error()};
            }

            duration = *std::move(result);
        }
    }

    trim_all_whitespace(input);

    if (!input.empty()) {
        return std::unexpected{ParseError{
          ParseError::Kind::BackgroundTrailingGarbage, original_input
        }};
    }

    return BackgroundShow{std::filesystem::path{path}, transition, duration};
}

auto Background::parse(std::string_view input) -> ParseResult<Background> {
    const auto original_input{input};

    if (!input.starts_with("> bg")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"> bg"}.size());

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    trim_leading_whitespace(input);

    const auto subcommand{next_token(input)};

    if (subcommand == "show") {
        return BackgroundShow::parse(input, original_input)
            .transform([](BackgroundShow show) -> Background {
                return Background{std::move(show)};
            });
    }

    if (subcommand == "hide") {
        trim_all_whitespace(input);

        if (!input.empty()) {
            return std::unexpected{ParseError{
              ParseError::Kind::BackgroundMalformedHide, original_input
            }};
        }

        return Background{BackgroundHide{}};
    }

    if (subcommand.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::BackgroundMissingCommand, original_input}
        };
    }

    return std::unexpected{
      ParseError{ParseError::Kind::BackgroundUnknownCommand, original_input}
    };
}

// MARK: - VariableMutation

VariableMutation::VariableMutation(
    Operation op, VariableName variable_name, EngineInt value
): op{op}, variable_name{variable_name}, value{value} {}

auto VariableMutation::parse(std::string_view input)
    -> ParseResult<VariableMutation> {
    const auto original_input{input};

    if (!input.starts_with("> set") && !input.starts_with("> add")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    const auto op{[&input] -> Operation {
        if (input.starts_with("> set")) {
            input.remove_prefix(std::string_view{"> set"}.size());
            return VariableMutation::Operation::Set;
        }

        assert(input.starts_with("> add"));
        input.remove_prefix(std::string_view{"> add"}.size());
        return VariableMutation::Operation::Add;
    }()};

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    const auto variable_name{next_token(input)};

    if (variable_name.empty()) {
        return std::unexpected{ParseError{
          ParseError::Kind::VariableMutationMissingVariableName, original_input
        }};
    }

    for (char i : variable_name) {
        if ((i < 'a' || i > 'z') && (i < 'A' || i > 'Z') && i != '-' && i != '_'
            && (i < '0' || i > '9')) {
            return std::unexpected{ParseError{
              ParseError::Kind::VariableMutationMalformedVariableName,
              original_input
            }};
        }
    }

    const auto value_str{next_token(input)};

    EngineInt value{0};

    auto [ptr, ec]{std::from_chars(
        value_str.data(), value_str.data() + value_str.size(), value
    )};

    if (ec == std::errc{} && ptr == value_str.data() + value_str.size()) {
        trim_all_whitespace(input);

        if (!input.empty()) {
            return std::unexpected{ParseError{
              ParseError::Kind::VariableMutationTrailingGarbage, original_input
            }};
        }

        return VariableMutation{op, variable_name, value};
    }

    return std::unexpected{ParseError{
      ParseError::Kind::VariableMutationMalformedValue, original_input
    }};
}

// MARK: - If

auto Atom::parse(std::string_view input, const std::string_view original_input)
    -> ParseResult<Atom> {
    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::IfAtomMissing, original_input}
        };
    }

    EngineInt value{0};

    auto [ptr, ec]{
      std::from_chars(input.data(), input.data() + input.size(), value)
    };

    if (ec == std::errc{} && ptr == input.data() + input.size()) {
        return Atom{value};
    }

    for (char i : input) {
        if ((i < 'a' || i > 'z') && (i < 'A' || i > 'Z') && i != '-' && i != '_'
            && (i < '0' || i > '9')) {
            return std::unexpected{
              ParseError{ParseError::Kind::IfAtomMalformed, original_input}
            };
        }
    }

    return Atom{input};
}

Operator::Operator(Value value): value{value} {}

auto Operator::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<Operator> {
    if (input == "==") {
        return Operator{Operator::Value::Equals};
    }

    if (input == "!=") {
        return Operator{Operator::Value::NotEquals};
    }

    if (input == ">") {
        return Operator{Operator::Value::GreaterThan};
    }

    if (input == "<") {
        return Operator{Operator::Value::LessThan};
    }

    if (input == ">=") {
        return Operator{Operator::Value::GreaterThanOrEqual};
    }

    if (input == "<=") {
        return Operator{Operator::Value::LessThanOrEqual};
    }

    return std::unexpected{
      ParseError{ParseError::Kind::IfUnknownOperator, original_input}
    };
}

If::If(Atom lhs, Operator op, Atom rhs): lhs{lhs}, op{op}, rhs{rhs} {}

auto If::parse(std::string_view input) -> ParseResult<If> {
    const auto original_input{input};

    if (!input.starts_with("> if")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"> if"}.size());

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    trim_leading_whitespace(input);

    const auto lhs_str{next_token(input)};
    auto lhs_result{Atom::parse(lhs_str, original_input)};

    if (!lhs_result) {
        return std::unexpected{std::move(lhs_result).error()};
    }

    trim_leading_whitespace(input);

    const auto op_str{next_token(input)};
    auto op_result{Operator::parse(op_str, original_input)};

    if (!op_result) {
        return std::unexpected{std::move(op_result).error()};
    }

    trim_leading_whitespace(input);

    const auto rhs_str{next_token(input)};
    auto rhs_result{Atom::parse(rhs_str, original_input)};

    if (!rhs_result) {
        return std::unexpected{std::move(rhs_result).error()};
    }

    trim_leading_whitespace(input);

    if (!input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::IfTrailingGarbage, original_input}
        };
    }

    return If{
      *std::move(lhs_result), *std::move(op_result), *std::move(rhs_result)
    };
}

auto EndIf::parse(std::string_view input) -> ParseResult<EndIf> {
    const auto original_input{input};

    if (!input.starts_with("> endif")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"> endif"}.size());

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    trim_all_whitespace(input);

    if (!input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::EndIfTrailingGarbage, original_input}
        };
    }

    return EndIf{};
}

// MARK: - Jump

Jump::Jump(Target target): target{std::move(target)} {}

auto Jump::parse(std::string_view input) -> ParseResult<Jump> {
    const auto original_input{input};

    if (!input.starts_with("> jmp")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"> jmp"}.size());

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    trim_leading_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::JumpEmptyTarget, original_input}
        };
    }

    if (!input.starts_with("[](")) {
        return std::unexpected{
          ParseError{ParseError::Kind::JumpMalformedTarget, original_input}
        };
    }

    input.remove_prefix(std::string_view{"[]("}.size());

    if (!input.ends_with(')')) {
        return std::unexpected{
          ParseError{ParseError::Kind::JumpMalformedTarget, original_input}
        };
    }

    input.remove_suffix(1);

    auto target_result{Target::parse(input, original_input)};

    if (!target_result) {
        return std::unexpected{std::move(target_result).error()};
    }

    return Jump{*std::move(target_result)};
}

// MARK: - Sprite

SpritePosition::SpritePosition(Value value): value{value} {}

auto SpritePosition::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<SpritePosition> {
    if (input == "left") {
        return SpritePosition{SpritePosition::Value::Left};
    }

    if (input == "center") {
        return SpritePosition{SpritePosition::Value::Center};
    }

    if (input == "right") {
        return SpritePosition{SpritePosition::Value::Right};
    }

    return std::unexpected{
      ParseError{ParseError::Kind::SpriteMalformedPosition, original_input}
    };
}

SpriteShow::SpriteShow(
    SpritePosition position,
    std::filesystem::path path,
    std::optional<Transition> transition,
    std::optional<Duration> duration
):
    position{position}, path{std::move(path)}, transition{transition},
    duration{duration} {
    normalize_transition_args(this->transition, this->duration);
}

auto SpriteShow::parse(
    std::string_view input, const std::string_view original_input
) -> ParseResult<SpriteShow> {
    trim_leading_whitespace(input);

    const auto position_str{next_token(input)};

    if (position_str.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::SpriteMissingPosition, original_input}
        };
    }

    auto position_result{SpritePosition::parse(position_str, original_input)};

    if (!position_result) {
        return std::unexpected{std::move(position_result).error()};
    }

    trim_leading_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::SpriteMissingPath, original_input}
        };
    }

    const auto path{next_token(input)};

    auto position{*std::move(position_result)};

    std::optional<Transition> transition;
    std::optional<Duration> duration;

    if (!input.empty()) {
        const auto transition_str{next_token(input)};

        auto result{Transition::parse(transition_str, original_input)};

        if (!result) {
            return std::unexpected{std::move(result).error()};
        }

        transition = *std::move(result);

        trim_leading_whitespace(input);

        if (!input.empty()) {
            const auto duration_str{next_token(input)};

            auto result{Duration::parse(duration_str, original_input)};

            if (!result) {
                return std::unexpected{std::move(result).error()};
            }

            duration = *std::move(result);
        }
    }

    trim_all_whitespace(input);

    if (!input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::SpriteTrailingGarbage, original_input}
        };
    }

    return SpriteShow{
      position, std::filesystem::path{path}, transition, duration
    };
}

SpriteHide::SpriteHide(SpritePosition position): position{position} {}

auto Sprite::parse(std::string_view input) -> ParseResult<Sprite> {
    const auto original_input{input};

    if (!input.starts_with("> sprite")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"> sprite"}.size());

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    trim_leading_whitespace(input);

    const auto subcommand{next_token(input)};

    if (subcommand == "show") {
        return SpriteShow::parse(input, original_input)
            .transform([](SpriteShow show) -> Sprite {
                return Sprite{std::move(show)};
            });
    }

    if (subcommand == "hide") {
        const auto position_str{next_token(input)};

        if (position_str.empty()) {
            return std::unexpected{ParseError{
              ParseError::Kind::SpriteMissingPosition, original_input
            }};
        }

        auto position{SpritePosition::parse(position_str, original_input)};

        if (!position) {
            return std::unexpected{std::move(position).error()};
        }

        trim_all_whitespace(input);

        if (!input.empty()) {
            return std::unexpected{ParseError{
              ParseError::Kind::SpriteTrailingGarbage, original_input
            }};
        }

        return Sprite{SpriteHide{*std::move(position)}};
    }

    if (subcommand.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::SpriteMissingCommand, original_input}
        };
    }

    return std::unexpected{
      ParseError{ParseError::Kind::SpriteUnknownCommand, original_input}
    };
}

// MARK: - Wait

Wait::Wait(Duration duration): duration{duration} {}

auto Wait::parse(std::string_view input) -> ParseResult<Wait> {
    const auto original_input{input};

    if (!input.starts_with("> wait")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"> wait"}.size());

    if (!input.empty() && input.front() != ' ' && input.front() != '\t') {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    const auto duration_str{next_token(input)};

    if (duration_str.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::WaitMissingDuration, original_input}
        };
    }

    auto duration_result{Duration::parse(duration_str, original_input)};

    if (!duration_result) {
        return std::unexpected{std::move(duration_result).error()};
    }

    trim_all_whitespace(input);

    if (!input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::WaitTrailingGarbage, original_input}
        };
    }

    return Wait{*std::move(duration_result)};
}

// MARK: - Choice

Choice::Choice(std::string_view prompt, Target target):
    prompt{prompt}, target{std::move(target)} {}

auto Choice::parse(std::string_view input) -> ParseResult<Choice> {
    const auto original_input{input};

    if (!input.starts_with("[")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(1);

    const auto closing_bracket_pos{input.find("](")};

    if (closing_bracket_pos == std::string_view::npos) {
        return std::unexpected{
          ParseError{ParseError::Kind::ChoiceMalformed, original_input}
        };
    }

    const auto prompt{input.substr(0, closing_bracket_pos)};

    if (prompt.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::ChoiceEmptyPrompt, original_input}
        };
    }

    input.remove_prefix(closing_bracket_pos + std::string_view{"]("}.size());

    if (!input.ends_with(')')) {
        return std::unexpected{
          ParseError{ParseError::Kind::ChoiceMalformed, original_input}
        };
    }

    input.remove_suffix(1);

    auto target_result{Target::parse(input, original_input)};

    if (!target_result) {
        return std::unexpected{std::move(target_result).error()};
    }

    return Choice{prompt, *std::move(target_result)};
}

// MARK: - Dialogue

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
Dialogue::Dialogue(std::string_view speaker, std::string_view text):
    speaker{speaker}, text{text} {}

auto Dialogue::parse(std::string_view input) -> ParseResult<Dialogue> {
    const auto original_input{input};

    if (!input.starts_with("**")) {
        return std::unexpected{
          ParseError{ParseError::Kind::UnknownPrefix, original_input}
        };
    }

    input.remove_prefix(std::string_view{"**"}.size());

    const auto speaker_end_pos{input.find("**:")};

    if (speaker_end_pos == std::string_view::npos) {
        return std::unexpected{
          ParseError{ParseError::Kind::DialogueMalformed, original_input}
        };
    }

    const auto speaker{input.substr(0, speaker_end_pos)};

    if (speaker.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::DialogueEmptySpeaker, original_input}
        };
    }

    input.remove_prefix(speaker_end_pos + std::string_view{"**:"}.size());

    trim_leading_whitespace(input);

    if (input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::DialogueMissingText, original_input}
        };
    }

    if (!input.starts_with('"')) {
        return std::unexpected{
          ParseError{ParseError::Kind::DialogueMalformed, original_input}
        };
    }

    input.remove_prefix(1);

    const auto text_end_pos{input.find_last_of('"')};

    if (text_end_pos == std::string_view::npos) {
        return std::unexpected{
          ParseError{ParseError::Kind::DialogueMalformed, original_input}
        };
    }

    const auto text{input.substr(0, text_end_pos)};

    if (text.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::DialogueEmptyText, original_input}
        };
    }

    input.remove_prefix(text_end_pos + 1);

    trim_all_whitespace(input);

    if (!input.empty()) {
        return std::unexpected{
          ParseError{ParseError::Kind::DialogueTrailingGarbage, original_input}
        };
    }

    return Dialogue{speaker, text};
}

// MARK: - Narration

Narration::Narration(std::string_view value): value{value} {}

auto Narration::parse(std::string_view input) -> ParseResult<Narration> {
    return Narration{input};
}

} // namespace parser
