#pragma once

#include <spdlog/spdlog.h>

#include "../../utils.hpp"

// --------------------------------------------------------------------------
struct Idle {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: Idle");
            ctx.idle();
        };
    }
};

// --------------------------------------------------------------------------
struct Error {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: Error");
            ctx.error(demangle(event) + " on state " + demangle(source));
        };
    }
};
