#pragma once

#include "Events.hpp"
#include "Guards.hpp"

#include "../Sequencer/Actions.hpp"

#include "../../utils.hpp"

#include <bhc/spdlog.hpp>
#include <hsm/hsm.h>

// States
struct Idle {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) { spdlog::info("ENTRY: Idle"); };
    }
};

struct Error {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) { spdlog::info("ENTRY: Error"); };
    }
};

struct GoToLoadingPose {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: GoToLoadingPose");
            spdlog::info("    --> Send go to loading pose to robot");
        };
    }
};

struct GoToLoadingPoseWaiting {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: GoToLoadingPoseWaiting");
        };
    }
};

struct LoadConfirmation {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: LoadConfirmation");
            spdlog::info("    --> Send load confirmation to ihm");
        };
    }
};

struct LoadConfirmationWaiting {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: LoadConfirmationWaiting");
        };
    }
};

struct CheckCalibrationPoint {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: CheckCalibrationPoint");
        };
    }
};

struct MoveToCalibrationPoint {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: MoveToCalibrationPoint");
            spdlog::info("    --> Send go to points {}", ctx.points[0]);
        };
    }
};

struct MoveToCalibrationPointWaiting {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: MoveToCalibrationPointWaiting");
            spdlog::info("    --> Remove a point");
            ctx.points.erase(ctx.points.begin());
        };
    }
};

struct SnapshotRequest {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: SnapshotRequest");
            spdlog::info("    --> Send a snapshot request");
        };
    }
};

struct SnapshotReplyWaiting {
    static constexpr auto on_entry()
    {
        return
            [](const auto& event, const auto& source, const auto& target, const auto& ctx) { spdlog::info("ENTRY: SnapshotReplyWaiting"); };
    }

    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: SnapshotReplyWaiting");
            spdlog::info("    --> Find Bevel");
        };
    }
};

struct ComputeTransform {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: ComputeTransform");
            spdlog::info("    --> Compute transform");
        };
    }
};

struct InitCalibration {
    static constexpr auto on_entry()
    {
        return
            [](const auto& event, const auto& source, const auto& target, const auto& ctx) { spdlog::info("ENTRY: InitStartCalibration"); };
    }
};
struct WaitingInitCalibrationIHM {};
struct WaitingInitCalibrationRobot {};

struct Calibration_IR_Robot {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, const auto& ctx) {
            spdlog::info("ENTRY: Calibration_IR_Robot");
            spdlog::info("    --> Send DisplayCalibration to IHM and Send go to home pose to Robot");
        };
    }

    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
              // Source                                 + Event                        [Guard]       / Action          = Target
              // +--------------------------------------+-----------------------------+--------------+-----------------+-----------------------------------------+
            * hsm::state<InitCalibration>               + hsm::event<ack_display_ihm>                / log_action = hsm::state<WaitingInitCalibrationRobot>,
              hsm::state<InitCalibration>               + hsm::event<ack_home_pose_robot>            / log_action = hsm::state<WaitingInitCalibrationIHM>,
              hsm::state<WaitingInitCalibrationRobot>   + hsm::event<ack_home_pose_robot>            / log_action = hsm::state<GoToLoadingPose>,
              hsm::state<WaitingInitCalibrationRobot>   + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<WaitingInitCalibrationIHM>     + hsm::event<ack_display_ihm>                / log_action = hsm::state<GoToLoadingPose>,
              hsm::state<WaitingInitCalibrationIHM>     + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<GoToLoadingPose>                                                            / log_action = hsm::state<GoToLoadingPoseWaiting>,
              hsm::state<GoToLoadingPoseWaiting>        + hsm::event<ack_load_pose_robot>            / log_action = hsm::state<LoadConfirmation>,
              hsm::state<GoToLoadingPoseWaiting>        + hsm::event<timeout>                        / log_action = hsm::state<Error>,
              hsm::state<LoadConfirmation>                                                           / log_action = hsm::state<LoadConfirmationWaiting>,
              hsm::state<LoadConfirmationWaiting>       + hsm::event<ack_load_confirmation_ihm>      / log_action = hsm::state<CheckCalibrationPoint>,
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
