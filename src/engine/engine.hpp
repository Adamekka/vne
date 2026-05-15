#pragma once

#include "interpreter.hpp"

namespace engine {

class Engine {
  public:
    explicit Engine(parser::ParsedScript script);

    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;

    ~Engine();

    auto operator=(const Engine&) -> Engine& = delete;
    auto operator=(Engine&&) -> Engine& = delete;

    auto run() -> void;

    std::unordered_map<std::string, Variable> variables;

  private:
    Interpreter interpreter;
};

} // namespace engine
