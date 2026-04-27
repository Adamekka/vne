#pragma once

namespace helpers {

// NOLINTNEXTLINE(readability-identifier-naming)
template<class... Ts> struct overloaded: Ts... {
    using Ts::operator()...;
};

} // namespace helpers
