/*
@startuml

[*] --> Idle
Idle --> StartCalibration : start_calibration
state GoToLoadingPose
StartCalibration : E: send_display_calibration_ihm\nE: send_go_to_home_robot
ComputeTransform: E: compute_transform
SnapshotReplyWaiting: X: find_bevel
SnapshotRequest: E: send_snapshot_request
MoveToCalibrationPointWaiting: E: remove_calibration_point
MoveToCalibrationPoint: E: send_to_go_point
GoToLoadingPose: E: send_go_to_loading_pose
LoadConfirmation: E: send_load_confirmation_ihm

state StartCalibration {

[*] --> InitStartCalibration
InitStartCalibration --> WaitingInitCalibrationRobot : ack_display_ihm
InitStartCalibration --> WaitingInitCalibrationIHM : ack_home_pose_robot
WaitingInitCalibrationRobot --> GoToLoadingPose : ack_home_pose_robot
WaitingInitCalibrationIHM --> GoToLoadingPose : ack_display_ihm
}
GoToLoadingPose --> GoToLoadingPoseWaiting
GoToLoadingPoseWaiting --> LoadConfirmation : ack_load_pose_robot
LoadConfirmation --> LoadConfirmationWaiting
LoadConfirmationWaiting --> CheckCalibrationPoint : ack_load_confirmation_ihm
CheckCalibrationPoint --> MoveToCalibrationPoint : 2-
CheckCalibrationPoint --> ComputeTransform : 1- [No_More_Points]
MoveToCalibrationPoint --> MoveToCalibrationPointWaiting
MoveToCalibrationPointWaiting --> SnapshotRequest : ack_move_point_robot
SnapshotRequest --> SnapshotReplyWaiting
SnapshotReplyWaiting --> CheckCalibrationPoint : ack_snapshot
ComputeTransform --> Idle

@enduml

*/

#include "utils.hpp"

#include <fmt/format.h>
#include <simple-hsm/simple-hsm.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace std;


// Context
struct CalibrationIrRobotContext {
    vector<int> calibration_points = {1, 2, 3, 4, 5};
};

// Events
struct calibration_ir_robot_start {};
struct ack_display_gui {};
struct ack_home_pose_robot {};
struct ack_load_pose_robot {};
struct ack_load_confirmation_gui {};
struct ack_move_point_robot {};
struct ack_snapshot {};

// Guards
const auto No_More_Points = [](const auto &event, auto &source, const auto & /* target */, const auto &dep) {
    cout << "GUARD: No_More_Points" << std::endl;
    return dep.calibration_points.empty();
};

// Actions
const auto log_action = [](auto event, auto source, auto target, const auto &ctx) {
    std::cout << fmt::format("State [{}] + Event [{}] = State [{}]", demangle(source), demangle(event), demangle(target)) << std::endl;
};

// States
struct Idle {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Idle" << std::endl;
        };
    }
};


struct GoToLoadingPose {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Go to loading pose:\n    --> Send go to loading pose to robot" << std::endl;
        };
    }
};

struct GoToLoadingPoseWaiting {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Go to loading pose waiting" << std::endl;
        };
    }
};

struct LoadConfirmation {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Load confirmation" << std::endl;
        };
    }
};

struct LoadConfirmationWaiting {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Load confirmation waiting" << std::endl;
        };
    }
};

struct CheckCalibrationPoint {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Check Calibration Points" << std::endl;
        };
    }
};

struct MoveToCalibrationPoint {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Move to calibration point\n   --> Send go to points " << ctx.calibration_points[0] << std::endl;
        };
    }
};

struct MoveToCalibrationPointWaiting {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, auto &ctx) {
            std::cout << "ENTRY: Move to calibration point waiting\n    --> Remove a point" << std::endl;
            ctx.calibration_points.erase(ctx.calibration_points.begin());
        };
    }
};

struct SnapshotRequest {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Snapshot request\n    --> Send a snapshot request" << std::endl;
        };
    }
};

struct SnapshotReplyWaiting {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Snapshot reply waiting" << std::endl;
        };
    }

    static constexpr auto on_exit() {
        return [](const auto &event, const auto &source, const auto &target, auto &ctx) {
            std::cout << "EXIT: Snapshot reply waiting\n    --> Find Bevel" << std::endl;
        };
    }
};

struct ComputeTransform {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, auto &ctx) {
            std::cout << "ENTRY: Compute Transform\n    --> Compute transform" << std::endl;
        };
    }
};

struct InitCalibrationWaiting {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Init start calibration" << std::endl;
        };
    }
};
struct InitCalibrationIHMWaiting {};
struct InitCalibrationRobotWaiting {};

struct Calibration_IR_Robot {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: start Init calibration:\n    --> Send DisplayCalibration to IHM and Send go to home pose to Robot" << std::endl;
        };
    }

    static constexpr auto make_transition_table() {
        // clang-format off
        return hsm::transition_table(
              // Source                                 + Event                         [Guard]  / Action  = Target
              // +--------------------------------------+-----------------------------+---------+----------------------+
            * hsm::state<InitCalibrationWaiting>          + hsm::event<ack_display_gui>            / log_action     = hsm::state<InitCalibrationRobotWaiting>,
              hsm::state<InitCalibrationWaiting>          + hsm::event<ack_home_pose_robot>        / log_action     = hsm::state<InitCalibrationIHMWaiting>,
              hsm::state<InitCalibrationRobotWaiting>   + hsm::event<ack_home_pose_robot>                         = hsm::state<GoToLoadingPose>,
              hsm::state<InitCalibrationIHMWaiting>     + hsm::event<ack_display_gui>                             = hsm::state<GoToLoadingPose>
        );
        // clang-format on
    }
};


// State Machine
struct StateMachineSequencer {
    static constexpr auto make_transition_table() {
        // clang-format off
        return hsm::transition_table(
                // Source                                 + Event                         [Guard]  / Action  = Target
                // +--------------------------------------+-----------------------------+---------+----------------------+
              * hsm::state<Idle>                          + hsm::event<calibration_ir_robot_start>         / log_action = hsm::state<Calibration_IR_Robot>,
                hsm::exit<Calibration_IR_Robot, GoToLoadingPose>                                      / log_action = hsm::state<GoToLoadingPoseWaiting>,
                hsm::state<GoToLoadingPoseWaiting>        + hsm::event<ack_load_pose_robot>       / log_action = hsm::state<LoadConfirmation>,
                hsm::state<LoadConfirmation>                                                      / log_action = hsm::state<LoadConfirmationWaiting>,
                hsm::state<LoadConfirmationWaiting>       + hsm::event<ack_load_confirmation_gui> / log_action = hsm::state<CheckCalibrationPoint>,
                hsm::state<CheckCalibrationPoint>                                                 / log_action = hsm::state<MoveToCalibrationPoint>,
                hsm::state<CheckCalibrationPoint>                                [No_More_Points] / log_action = hsm::state<ComputeTransform>,
                hsm::state<MoveToCalibrationPoint>                                                / log_action = hsm::state<MoveToCalibrationPointWaiting>,
                hsm::state<MoveToCalibrationPointWaiting> + hsm::event<ack_move_point_robot>      / log_action = hsm::state<SnapshotRequest>,
                hsm::state<SnapshotRequest>                                                       / log_action = hsm::state<SnapshotReplyWaiting>,
                hsm::state<SnapshotReplyWaiting>          + hsm::event<ack_snapshot>              / log_action = hsm::state<CheckCalibrationPoint>,
                hsm::state<ComputeTransform>                                                      / log_action = hsm::state<Idle>
        );

        // clang-format on
    }

    static constexpr auto on_unexpected_event() {
        return [](auto &event, const auto &state, const auto &ctx) {
            std::cout << "Unexpected event: " << demangle(event) << " for state: " << demangle(state) << std::endl;
        };
    }
};

int main() {
    CalibrationIrRobotContext context;

    hsm::sm<StateMachineSequencer, CalibrationIrRobotContext> sm{context};

    std::string command;
    do {
        cout << "s: start_calibration\n";
        cout << "ai: ack display ihm\n";
        cout << "ar: ack home pose robot\n";
        cout << "alp: ack load pose robot\n";
        cout << "alc: ack load confirm ihm\n";
        cout << "amp: ack move to point\n";
        cout << "as: ack snapshot request\n";
        cout << "r: reset\n";
        cout << "q: quit\n";
        cout << "\n> ";
        cout.flush();

        if (!getline(cin, command)) {
            break;
        }

        if (command == "s") {
            sm.process_event(calibration_ir_robot_start{});
        } else if (command == "r") {
            sm = {context};
        } else if (command == "ai") {
            sm.process_event(ack_display_gui{});
        } else if (command == "ar") {
            sm.process_event(ack_home_pose_robot{});
        } else if (command == "alp") {
            sm.process_event(ack_load_pose_robot{});
        } else if (command == "alc") {
            sm.process_event(ack_load_confirmation_gui{});
        } else if (command == "amp") {
            sm.process_event(ack_move_point_robot{});
        } else if (command == "as") {
            sm.process_event(ack_snapshot{});
        }

    } while (command != "q");
}
