#include <hsm/hsm.h>

#include <iostream>
#include <string>

using namespace boost::hana;
using namespace std;


// Context
struct CalibrationContext {
};

// Events
struct start_calibration {
};

struct ack_display_ihm {};
struct ack_home_pose_robot {};
struct ack_load_pose_robot {};
struct ack_load_confirmation_ihm {};

// Guards
//const auto Expired = [](const auto &event, auto &source, const auto & /* target */, auto &dep) {
//    source.remaining -= event.elapsed_time;
//    return (source.remaining < 0s);
//};

// Actions
const auto log = [](auto event, auto source, auto target, const auto &ctx, const char *msg = "") {
    std::cout << msg << boost::hana::experimental::print(typeid_(source)) << " + "
              << boost::hana::experimental::print(typeid_(event)) << " = "
              << boost::hana::experimental::print(typeid_(target)) << std::endl;
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

struct MoveToCalibrationPoint {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: Move to calibration point" << std::endl;
        };
    }
};

struct InitStartCalibration {};
struct WaitingInitCalibrationIHM {};
struct WaitingInitCalibrationRobot {};

struct StartCalibration {
    static constexpr auto on_entry() {
        return [](const auto &event, const auto &source, const auto &target, const auto &ctx) {
            std::cout << "ENTRY: start Init calibration:\n    --> Send DisplayCalibration to IHM and Send go to home pose to Robot" << std::endl;
        };
    }

    static constexpr auto make_transition_table() {
        // clang-format off
        return hsm::transition_table(
            * hsm::state<InitStartCalibration> + hsm::event<ack_display_ihm> / log = hsm::state<WaitingInitCalibrationRobot>,
              hsm::state<InitStartCalibration> + hsm::event<ack_home_pose_robot> / log = hsm::state<WaitingInitCalibrationIHM>,
              hsm::state<WaitingInitCalibrationRobot> + hsm::event<ack_home_pose_robot> = hsm::state<GoToLoadingPose>,
              hsm::state<WaitingInitCalibrationIHM> + hsm::event<ack_display_ihm> = hsm::state<GoToLoadingPose>
        );
        // clang-format on
    }
};


// State Machine
struct CalibrationSm {
    static constexpr auto make_transition_table() {
        // clang-format off
        return hsm::transition_table(
            // Source              + Event            [Guard]   / Action  = Target
            // +-------------------+------------------+---------+---------+----------------------+
            * hsm::state<Idle>     + hsm::event<start_calibration>  / log = hsm::state<StartCalibration>,
              hsm::exit<StartCalibration, GoToLoadingPose>          / log = hsm::state<GoToLoadingPoseWaiting>,
              hsm::state<GoToLoadingPoseWaiting> + hsm::event<ack_load_pose_robot> / log = hsm::state<LoadConfirmation>,
              hsm::state<LoadConfirmation>                          / log = hsm::state<LoadConfirmationWaiting>,
              hsm::state<LoadConfirmationWaiting> + hsm::event<ack_load_confirmation_ihm> / log = hsm::state<MoveToCalibrationPoint>
            );

        // clang-format on
    }

    static constexpr auto on_unexpected_event() {
        return [](auto &event, const auto &state, const auto &ctx) {
            std::cout << "Unexpected event: " << boost::hana::experimental::print(typeid_(event))
                      << " for state: " << boost::hana::experimental::print(typeid_(state)) << std::endl;
        };
    }
};

int main() {
    CalibrationContext context;

    hsm::sm<CalibrationSm, CalibrationContext> sm{context};

    std::string command;
    do {
        cout << "s: start_calibration\n";
        cout << "ai: ack display ihm\n";
        cout << "ar: ack home pose robot\n";
        cout << "alp: ack load pose robot\n";
        cout << "alc: ack load confirm ihm\n";
        cout << "q: quit\n";
        cout << "\n> ";
        cout.flush();

        if (!getline(cin, command)) {
            break;
        }

        if (command == "s") {
            sm.process_event(start_calibration{});
        } else if (command == "ai") {
            sm.process_event(ack_display_ihm{});
        } else if (command == "ar") {
            sm.process_event(ack_home_pose_robot{});
        } else if (command == "alp") {
            sm.process_event(ack_load_pose_robot{});
        } else if (command == "alc") {
            sm.process_event(ack_load_confirmation_ihm{});
        }

    } while (command != "q");
}
