#include "../helpers/overloaded.hpp"
#include "../parser/parser.hpp"
#include "audio_manager.hpp"
#include "background_manager.hpp"
#include "choice_manager.hpp"
#include "dialogue_manager.hpp"
#include "engine.hpp"
#include "sprite_manager.hpp"
#include <ranges>
#include <thread>

namespace engine {

Interpreter::Interpreter(Engine& engine, parser::ParsedScript script)
    : engine{engine}
    , script{std::move(script)} {
    this->prepare_script();
}

auto Interpreter::new_script(parser::ParsedScript script) -> void {
    this->script = std::move(script);
    this->instruction_index = 0;
    this->labels.clear();
    this->skipping_stack.clear();
    this->prepare_script();
}

auto Interpreter::prepare_script() -> void {
    const auto& instructions{this->script.get_instructions()};

    if (instructions.empty()) {
        throw std::runtime_error{"Script must contain a version instruction"};
    }

    const auto& first_instruction{instructions.front()};

    if (!std::holds_alternative<parser::Version>(first_instruction)) {
        throw std::runtime_error{
          "First instruction must be a version instruction"
        };
    }

    for (auto it{std::next(instructions.begin())}; it != instructions.end();
         it++) {
        const auto& instruction{*it};

        std::visit(
            helpers::overloaded{
              [](const parser::Version&) -> void {
                  throw std::runtime_error{
                    "Only one version instruction is allowed"
                  };
              },

              [this, &instructions, &it](const parser::Label& label) -> void {
                  auto label_name{std::string{label.value}};

                  if (this->labels.contains(label_name)) {
                      throw std::runtime_error{
                        "Duplicate label: " + std::move(label_name)
                      };
                  }

                  this->labels.emplace(
                      std::move(label_name),
                      std::distance(instructions.begin(), it)
                  );
              },

              [](const parser::Audio&) -> void {},

              [](const parser::Background&) -> void {},

              [](const parser::VariableMutation&) -> void {},

              [](const parser::If&) -> void {},

              [](const parser::EndIf&) -> void {},

              [](const parser::Jump&) -> void {},

              [](const parser::Sprite&) -> void {},

              [](const parser::Wait&) -> void {},

              [](const parser::Choice&) -> void {},

              [](const parser::Dialogue&) -> void {},

              [](const parser::Narration&) -> void {}
            },
            instruction
        );

        if (std::holds_alternative<parser::Version>(instruction)) {
        }
    }
}

auto Interpreter::go_to_target(const parser::Target& target) -> void {
    std::visit(
        helpers::overloaded{
          [this](const parser::LabelValue& label) -> void {
              auto label_it{this->labels.find(std::string(label))};

              if (label_it == this->labels.end()) {
                  throw std::runtime_error{"Label not found"};
              }

              this->instruction_index = label_it->second;
          },
          [this](const parser::Target::File& path) -> void {
              this->new_script(parser::Parser{path}.parse());
          }
        },
        target.target
    );
}

auto Interpreter::select_choice(const parser::Choice& choice) -> void {
    const parser::Target target{choice.target};

    ChoiceManager::instance().clear();
    this->choice_block = false;
    this->go_to_target(target);
    this->next();
}

[[nodiscard]] auto Interpreter::interpolate_text(std::string_view text) const
    -> std::string {
    std::string result;
    size_t cursor{0};

    while (cursor < text.size()) {
        const size_t open_brace{text.find('{', cursor)};

        if (open_brace == std::string_view::npos) {
            result.append(text.substr(cursor));
            break;
        }

        result.append(text.substr(cursor, open_brace - cursor));

        const size_t close_brace{text.find('}', open_brace + 1)};

        if (close_brace == std::string_view::npos) {
            result.append(text.substr(open_brace));
            break;
        }

        const std::string variable_name{
          text.substr(open_brace + 1, close_brace - open_brace - 1)
        };
        const auto variable{this->engine.variables.find(variable_name)};

        if (variable == this->engine.variables.end()) {
            // Missing script variables intentionally match default state value.
            result += "0";
        } else {
            result += std::to_string(variable->second.value);
        }

        cursor = close_brace + 1;
    }

    return result;
}

// NOLINTNEXTLINE(misc-no-recursion)
auto Interpreter::next() -> void {
    if (this->choice_block) {
        return;
    }

    if (this->instruction_index + 1 >= this->script.get_instructions().size()) {
        // End of script
        exit(0);
    }

    this->instruction_index++;

    const auto& instruction{
      this->script.get_instructions().at(this->instruction_index)
    };

    auto handle_endif{[this]() -> void {
        if (!this->skipping_stack.empty()) {
            this->skipping_stack.pop_back();
        }
    }};

    // If we are in a skipping state (If false)
    if (std::ranges::find(this->skipping_stack, false)
        != this->skipping_stack.end()) {
        if (std::holds_alternative<parser::If>(instruction)) {
            this->skipping_stack.emplace_back(false);
        }

        if (std::holds_alternative<parser::EndIf>(instruction)) {
            handle_endif();
        }

        this->next();
        return;
    }

    if (!ChoiceManager::instance().get_choices().empty()) {
        if (std::holds_alternative<parser::Choice>(instruction)) {
            ChoiceManager::instance().show(
                std::get<parser::Choice>(instruction)
            );
            this->next();
            return;
        }

        if (!(std::holds_alternative<parser::If>(instruction)
              || std::holds_alternative<parser::EndIf>(instruction))) {
            this->choice_block = true;
            return;
        }
    }

    std::visit(
        helpers::overloaded{
          [this](const parser::Version&) -> void {
              // Multiple versions are handled in the constructor
              this->next();
          },

          [this](const parser::Label&) -> void {
              // Indexing labels is handled in the constructor
              this->next();
          },

          [this](const parser::Audio& audio) -> void {
              std::visit(
                  helpers::overloaded{
                    [](const parser::AudioPlay& play) -> void {
                        AudioManager::instance().play(play);
                    },

                    [](const parser::AudioResume&) -> void {
                        AudioManager::instance().resume();
                    },

                    [](const parser::AudioPause&) -> void {
                        AudioManager::instance().pause();
                    },

                    [](const parser::AudioStop&) -> void {
                        AudioManager::instance().stop();
                    }
                  },
                  audio.action
              );

              this->next();
          },

          [this](const parser::Background& background) -> void {
              std::visit(
                  helpers::overloaded{
                    [](const parser::BackgroundShow& show) -> void {
                        BackgroundManager::instance().show(show);
                    },

                    [](const parser::BackgroundHide&) -> void {
                        BackgroundManager::instance().hide();
                    }
                  },
                  background.value
              );

              this->next();
          },

          [this](const parser::VariableMutation& mutation) -> void {
              const auto var{this->engine.variables
                                 .try_emplace(
                                     std::string{mutation.variable_name},
                                     Variable{0}
                                 )
                                 .first};

              switch (mutation.op) {
                  case parser::VariableMutation::Operation::Set: {
                      var->second.value = mutation.value;
                  } break;

                  case parser::VariableMutation::Operation::Add: {
                      var->second.value += mutation.value;
                  } break;
              }

              this->next();
          },

          [this](const parser::If& condition) -> void {
              auto get_value{
                [this](const parser::Atom& atom) -> Variable::Type {
                    return std::visit(
                        helpers::overloaded{
                          [this](const parser::VariableName& name)
                              -> Variable::Type {
                              auto var = this->engine.variables.find(
                                  std::string(name)
                              );

                              if (var != this->engine.variables.end()) {
                                  return var->second.value;
                              }

                              return Variable::Type{};
                          },

                          [](const parser::EngineInt& value) -> Variable::Type {
                              return value;
                          }
                        },
                        atom.value
                    );
                }
              };

              auto lhs_value{get_value(condition.lhs)};
              auto rhs_value{get_value(condition.rhs)};

              this->skipping_stack.emplace_back([&]() -> bool {
                  switch (condition.op.value) {
                      case parser::Operator::Value::Equals: {
                          return lhs_value == rhs_value;
                      } break;

                      case parser::Operator::Value::NotEquals: {
                          return lhs_value != rhs_value;
                      } break;

                      case parser::Operator::Value::GreaterThan: {
                          return lhs_value > rhs_value;
                      } break;

                      case parser::Operator::Value::LessThan: {
                          return lhs_value < rhs_value;
                      } break;

                      case parser::Operator::Value::GreaterThanOrEqual: {
                          return lhs_value >= rhs_value;
                      } break;

                      case parser::Operator::Value::LessThanOrEqual: {
                          return lhs_value <= rhs_value;
                      } break;

                      default: {
                          throw std::runtime_error{"Unknown operator"};
                      } break;
                  }
              }());

              this->next();
          },

          [this, handle_endif](const parser::EndIf&) -> void {
              handle_endif();

              this->next();
          },

          [this](const parser::Jump& jump) -> void {
              this->go_to_target(jump.target);

              this->next();
          },

          [this](const parser::Sprite& sprite) -> void {
              std::visit(
                  helpers::overloaded{
                    [](const parser::SpriteShow& sprite_info) -> void {
                        SpriteManager::instance().show(sprite_info);
                    },

                    [](const parser::SpriteHide& sprite_info) -> void {
                        SpriteManager::instance().hide(sprite_info);
                    }
                  },
                  sprite.value
              );

              this->next();
          },

          [this](const parser::Wait& wait) -> void {
              std::this_thread::sleep_for(
                  std::chrono::duration<double>(wait.duration.value)
              );

              this->next();
          },

          [this](const parser::Choice& choice) -> void {
              ChoiceManager::instance().show(choice);

              this->next();
          },

          [this](const parser::Dialogue& dialogue) -> void {
              DialogueManager::instance().show_dialogue(
                  std::string{dialogue.speaker},
                  this->interpolate_text(dialogue.text)
              );
          },

          [this](const parser::Narration& narration) -> void {
              DialogueManager::instance().show_narration(
                  this->interpolate_text(narration.value)
              );
          }
        },
        instruction
    );
}

} // namespace engine
