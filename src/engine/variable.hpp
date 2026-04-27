#pragma once

#include <cstdint>

namespace engine {

class Variable {
  public:
    using Type = int64_t;

    Type value{0};

    explicit Variable(Type value);
};

}; // namespace engine
