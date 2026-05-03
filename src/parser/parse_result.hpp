#pragma once

#include <expected>
#include <string>

namespace parser {

struct ParseError {
    enum class Kind : uint8_t {
        /// The input didn't match any known instruction.
        UnknownPrefix,

        // MARK: Duration
        DurationMissingNumber,
        DurationMalformedNumber,

        // MARK: Transition
        TransitionMalformed,

        // MARK: Target
        TargetMalformed,
        TargetWithSpace,

        // MARK: Label
        LabelEmpty,
        LabelWithSpace,
        LabelMalformed,

        // MARK: Audio
        AudioMissingPath,
        AudioMalformedLoopFlag,
        AudioMalformedResume,
        AudioMalformedPause,
        AudioMalformedStop,
        AudioMissingCommand,
        AudioUnknownCommand,

        // MARK: Background
        BackgroundMissingPath,
        BackgroundMalformedHide,
        BackgroundTrailingGarbage,
        BackgroundMissingCommand,
        BackgroundUnknownCommand,

        // MARK: VariableMutation
        VariableMutationMissingVariableName,
        VariableMutationMalformedVariableName,
        VariableMutationMalformedValue,
        VariableMutationTrailingGarbage,

        // MARK: If
        IfAtomMissing,
        IfAtomMalformed,
        IfUnknownOperator,
        IfTrailingGarbage,
        EndIfTrailingGarbage,

        // MARK: Jump
        JumpEmptyTarget,
        JumpMalformedTarget,

        // MARK: Sprite
        SpriteMalformedPosition,
        SpriteMissingPosition,
        SpriteMissingPath,
        SpriteMalformedHide,
        SpriteTrailingGarbage,
        SpriteMissingCommand,
        SpriteUnknownCommand,

        // MARK: Wait
        WaitMissingDuration,
        WaitTrailingGarbage,

        // MARK: Choice
        ChoiceMalformed,
        ChoiceEmptyPrompt,

        // MARK: Dialogue
        DialogueMalformed,
        DialogueEmptySpeaker,
        DialogueMissingText,
        DialogueEmptyText,
        DialogueTrailingGarbage,
    } kind;
    std::string_view input;

    ParseError(Kind kind, std::string_view input);

    [[nodiscard]] auto message() const -> std::string;
};

template<typename T> using ParseResult = std::expected<T, ParseError>;

} // namespace parser
