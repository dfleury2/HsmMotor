#pragma once

#include <tuple>

namespace hsm {

template <class... Events>
constexpr auto events = std::tuple<Events...>{};

}
