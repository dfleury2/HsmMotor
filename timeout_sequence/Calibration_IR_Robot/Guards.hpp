#pragma once

#include <spdlog/spdlog.h>

namespace calibration_ir_robot {

// Guards
const auto No_More_Points = [](const auto& event, auto& source, const auto& /* target */, const auto& ctx) {
    spdlog::info("GUARD: No_More_Points");
    return ctx.calibration_points.empty();
};

}
