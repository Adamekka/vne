#include "parse_result.hpp"
#include <cassert>

namespace parser {

ParseError::ParseError(Kind kind, std::string_view input):
    kind{kind}, input{input} {}

auto ParseError::message() const -> std::string {
    const auto description{[this] -> std::string_view {
        switch (this->kind) {
            case Kind::UnknownPrefix: return "Unknown instruction prefix";

            case Kind::DurationMissingNumber:
                return "Duration is missing a number";
            case Kind::DurationMalformedNumber:
                return "Duration has a malformed number";

            case Kind::TransitionMalformed: return "Transition is malformed";

            case Kind::TargetMalformed: return "Target is malformed";
            case Kind::TargetWithSpace: return "Target contains whitespace";

            case Kind::LabelEmpty: return "Label is empty";
            case Kind::LabelWithSpace: return "Label contains whitespace";
            case Kind::LabelMalformed: return "Label is malformed";

            case Kind::AudioMissingPath:
                return "Audio command is missing a path";
            case Kind::AudioMalformedLoopFlag:
                return "Audio loop flag is malformed";
            case Kind::AudioMalformedResume:
                return "Audio resume command is malformed";
            case Kind::AudioMalformedPause:
                return "Audio pause command is malformed";
            case Kind::AudioMalformedStop:
                return "Audio stop command is malformed";
            case Kind::AudioMissingCommand:
                return "Audio command is missing a subcommand";
            case Kind::AudioUnknownCommand:
                return "Audio command has an unknown subcommand";

            case Kind::BackgroundMissingPath:
                return "Background command is missing a path";
            case Kind::BackgroundMalformedHide:
                return "Background hide command is malformed";
            case Kind::BackgroundTrailingGarbage:
                return "Background command has trailing garbage";
            case Kind::BackgroundMissingCommand:
                return "Background command is missing a subcommand";
            case Kind::BackgroundUnknownCommand:
                return "Background command has an unknown subcommand";

            case Kind::VariableMutationMissingVariableName:
                return "Variable mutation is missing a variable name";
            case Kind::VariableMutationMalformedVariableName:
                return "Variable mutation variable name is malformed";
            case Kind::VariableMutationMalformedValue:
                return "Variable mutation value is malformed";
            case Kind::VariableMutationTrailingGarbage:
                return "Variable mutation has trailing garbage";

            case Kind::IfAtomMissing: return "If expression is missing an atom";
            case Kind::IfAtomMalformed:
                return "If expression atom is malformed";
            case Kind::IfUnknownOperator:
                return "If expression has an unknown operator";
            case Kind::IfTrailingGarbage:
                return "If expression has trailing garbage";
            case Kind::EndIfTrailingGarbage:
                return "Endif command has trailing garbage";

            case Kind::JumpEmptyTarget:
                return "Jump command has an empty target";
            case Kind::JumpMalformedTarget: return "Jump target is malformed";

            case Kind::SpriteMalformedPosition:
                return "Sprite position is malformed";
            case Kind::SpriteMissingPosition:
                return "Sprite command is missing a position";
            case Kind::SpriteMissingPath:
                return "Sprite command is missing a path";
            case Kind::SpriteMalformedHide:
                return "Sprite hide command is malformed";
            case Kind::SpriteTrailingGarbage:
                return "Sprite command has trailing garbage";
            case Kind::SpriteMissingCommand:
                return "Sprite command is missing a subcommand";
            case Kind::SpriteUnknownCommand:
                return "Sprite command has an unknown subcommand";

            case Kind::WaitMissingDuration:
                return "Wait command is missing a duration";
            case Kind::WaitTrailingGarbage:
                return "Wait command has trailing garbage";

            case Kind::ChoiceMalformed: return "Choice is malformed";
            case Kind::ChoiceEmptyPrompt: return "Choice prompt is empty";

            case Kind::DialogueMalformed: return "Dialogue is malformed";
            case Kind::DialogueEmptySpeaker: return "Dialogue speaker is empty";
            case Kind::DialogueMissingText: return "Dialogue is missing text";
            case Kind::DialogueEmptyText: return "Dialogue text is empty";
            case Kind::DialogueTrailingGarbage:
                return "Dialogue has trailing garbage";
        }

        assert(false);
    }()};

    std::string result{description};

    result += ": '";
    result += this->input;
    result += '\'';

    return result;
}

} // namespace parser
