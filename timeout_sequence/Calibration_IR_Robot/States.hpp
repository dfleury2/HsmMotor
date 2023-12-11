#pragma once

#include "Events.hpp"
#include "Guards.hpp"

#include "../Sequencer/Actions.hpp"

#include "../../utils.hpp"

#include <hsm/hsm.h>
#include <spdlog/spdlog.h>

// --------------------------------------------------------------------------
// States
// --------------------------------------------------------------------------
struct GoToLoadingPose {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: GoToLoadingPose");
            ctx.send_go_loading_pose();
        };
    }
};

// --------------------------------------------------------------------------
struct GoToLoadingPoseWaiting {
    inline static auto TTL = std::chrono::seconds{5};

    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: GoToLoadingPoseWaiting");
            ctx.start_timer(TTL);
        };
    }

    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: GoToLoadingPoseWaiting");
            ctx.cancel_timer();
        };
    }
};

// --------------------------------------------------------------------------
struct LoadConfirmation {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: LoadConfirmation");
            ctx.send_load_confirmation();
        };
    }
};

// --------------------------------------------------------------------------
struct LoadConfirmationWaiting {
    inline static auto TTL = std::chrono::minutes{5};

    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: LoadConfirmationWaiting");
            ctx.start_timer(TTL);
        };
    }
    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: LoadConfirmationWaiting");
            ctx.cancel_timer();
        };
    }
};

// --------------------------------------------------------------------------
struct CheckCalibrationPoint {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) { spdlog::info("ENTRY: CheckCalibrationPoint"); };
    }
};

// --------------------------------------------------------------------------
struct MoveToCalibrationPoint {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: MoveToCalibrationPoint");
            ctx.send_go_to_point();
        };
    }
};

// --------------------------------------------------------------------------
struct MoveToCalibrationPointWaiting {
    inline static auto TTL = std::chrono::seconds{5};

    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: MoveToCalibrationPointWaiting");
            ctx.remove_point();
            ctx.start_timer(TTL);
        };
    }

    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: SnapshotReplyWaiting");
            ctx.cancel_timer();
            ctx.find_bevel();
        };
    }
};

// --------------------------------------------------------------------------
struct SnapshotRequest {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: SnapshotRequest");
            ctx.send_snapshot_request();
        };
    }
};

// --------------------------------------------------------------------------
struct SnapshotReplyWaiting {
    inline static auto TTL = std::chrono::seconds{2};

    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: SnapshotReplyWaiting");
            ctx.start_timer(TTL);
        };
    }

    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: SnapshotReplyWaiting");
            spdlog::info("    --> Find Bevel");
            ctx.cancel_timer();
        };
    }
};

// --------------------------------------------------------------------------
struct ComputeTransform {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: ComputeTransform");
            ctx.compute_transform();
        };
    }
};

// --------------------------------------------------------------------------
struct InitCalibrationWaiting {
    inline static auto TTL = std::chrono::seconds{5};

    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: InitCalibrationWaiting");
            ctx.start_timer(TTL);
        };
    }
};

// --------------------------------------------------------------------------
struct InitCalibrationIHMWaiting {
    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: InitCalibrationIHMWaiting");
            ctx.cancel_timer();
        };
    }
};

// --------------------------------------------------------------------------
struct InitCalibrationRobotWaiting {
    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: InitCalibrationRobotWaiting");
            ctx.cancel_timer();
        };
    }
};

// --------------------------------------------------------------------------
struct Calibration_IR_Robot {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: Calibration_IR_Robot");
            ctx.send_display_calibration_screen();
            ctx.send_go_home();
        };
    }
    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) { spdlog::info("EXIT: Calibration_IR_Robot"); };
    }

    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
              // Source                                 + Event                        [Guard]       / Action          = Target
              // +--------------------------------------+-----------------------------+--------------+-----------------+-----------------------------------------+
            * hsm::state<InitCalibrationWaiting>        + hsm::event<ack_display_gui>                / log_action = hsm::state<InitCalibrationRobotWaiting>,
              hsm::state<InitCalibrationWaiting>        + hsm::event<ack_home_pose_robot>            / log_action = hsm::state<InitCalibrationIHMWaiting>,
              hsm::state<InitCalibrationWaiting>        + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<InitCalibrationRobotWaiting>   + hsm::event<ack_home_pose_robot>            / log_action = hsm::state<GoToLoadingPose>,
              hsm::state<InitCalibrationRobotWaiting>   + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<InitCalibrationIHMWaiting>     + hsm::event<ack_display_gui>                / log_action = hsm::state<GoToLoadingPose>,
              hsm::state<InitCalibrationIHMWaiting>     + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<GoToLoadingPose>                                                            / log_action = hsm::state<GoToLoadingPoseWaiting>,
              hsm::state<GoToLoadingPoseWaiting>        + hsm::event<ack_load_pose_robot>            / log_action = hsm::state<LoadConfirmation>,
              hsm::state<GoToLoadingPoseWaiting>        + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<LoadConfirmation>                                                           / log_action = hsm::state<LoadConfirmationWaiting>,
              hsm::state<LoadConfirmationWaiting>       + hsm::event<ack_load_confirmation_gui>      / log_action = hsm::state<CheckCalibrationPoint>,
              hsm::state<LoadConfirmationWaiting>       + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<CheckCalibrationPoint>                                                      / log_action = hsm::state<MoveToCalibrationPoint>,
              hsm::state<CheckCalibrationPoint>                                     [No_More_Points] / log_action = hsm::state<ComputeTransform>,
              hsm::state<MoveToCalibrationPoint>                                                     / log_action = hsm::state<MoveToCalibrationPointWaiting>,
              hsm::state<MoveToCalibrationPointWaiting> + hsm::event<ack_move_point_robot>           / log_action = hsm::state<SnapshotRequest>,
              hsm::state<MoveToCalibrationPointWaiting> + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<SnapshotRequest>                                                            / log_action = hsm::state<SnapshotReplyWaiting>,
              hsm::state<SnapshotReplyWaiting>          + hsm::event<ack_snapshot>                   / log_action = hsm::state<CheckCalibrationPoint>,
              hsm::state<SnapshotReplyWaiting>          + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<ComputeTransform>                                                           / log_action = hsm::state<Idle>
        );
        // clang-format on
    }
};
