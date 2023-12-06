#pragma once

#include <bhc/spdlog.hpp>

// Guards
const auto No_More_Points = [](const auto& event, auto& source, const auto& /* target */, const auto& ctx) {
    spdlog::info("GUARD: No_More_Points");
    return ctx.points.empty();
};
