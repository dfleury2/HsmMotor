#pragma once

#include <spdlog/spdlog.h>

#include "../../utils.hpp"

// Actions
const auto log_action = [](auto event, auto source, auto target, const auto& ctx) {
    spdlog::info("TRANSITION: State [{}] {} => State [{}]", demangle(source),
                 (demangle(event) == "hsm::noneEvent" ? "" : fmt::format("+ Event [{}]", demangle(event))), demangle(target));
};
