#pragma once

#include <iostream>

namespace hsm {

constexpr auto log = [](const auto& event, const auto& source, const auto& target, const auto&...) {
    std::cout << "a log is done ... " << std::endl;
};
}
