#pragma once

#include "StepperMotor.hpp"

#include "utils.hpp"

#include <hsm/hsm.h>

#define ON_ENTRY()                                                                                           \
    static constexpr auto on_entry() {                                                                                \
        return [](const auto &event, const auto &source, const auto &target, auto &dep) {                             \
            std::cout << "      -- State [" << demangle(source) << "]: on_entry: State [" << demangle(source) << "] --> State [" << demangle(target) << "]" << std::endl; \
        };                                                                                                            \
    }

#define ON_EXIT()                                                                                           \
    static constexpr auto on_exit() {                                                                                \
        return [](const auto &event, const auto &source, const auto &target, auto &dep) {                            \
            std::cout << "      -- State [" << demangle(source) << "]: on_exit: State [" << demangle(source) << "] --> State [" << demangle(target) << "]" << std::endl; \
        };                                                                                                           \
    }

#define ON()   \
    ON_ENTRY() \
    ON_EXIT()


namespace action {
    // --------------------------------------------------------------------------
    // States
    struct Off {ON();};
    struct On {ON();};
    struct Idle {ON();};
    struct Busy {ON();};
    struct BusyContinuous {ON();};

    // --------------------------------------------------------------------------
    // Events
    struct enable {};
    struct disable {};
    struct stop {};
    struct step_done {};
    struct rotate_step {
        int steps = 0;
    };
    struct rotate_continuous {};

    const auto Logger = [](const std::string& type, const auto& event, const auto& source, const auto& target) {
        std::cout << "      -- " << type << ": Event [" << demangle(event) << "], source [" << demangle(source) << "], target [" << demangle(target) << "]" << std::endl; \
    };


    // --------------------------------------------------------------------------
    // Guards
    const auto NoMoreSteps = [](const auto & event, auto & source, const auto & target, const auto &dep) {
        static const std::string str = "Guard: ";

        auto guard = (dep.getRemainingSteps() <= 0);

        Logger(str + (guard ? "true" : "false"), event, source, target);
        return guard;
    };

    // --------------------------------------------------------------------------
    // Actions
    const auto RotateStepMotor = [](const auto &event, const auto & source, const auto & target, auto &dep) {
        Logger("Action", event, source, target);
        dep.rotateStep(event.steps);
    };
    const auto RotateContinuousMotor = [](const auto &event, const auto & source, const auto & target, auto &dep) {
        Logger("Action", event, source, target);
        dep.rotateContinuous();
    };
    const auto StopMotor = [](const auto & event, const auto & source, const auto & target, auto &dep) {
        Logger("Action", event, source, target);
        dep.stopMotor();
    };
    const auto StepDone = [](const auto & event, const auto & source, const auto & target, auto &dep) {
        Logger("Action", event, source, target);
        dep.stepDone();
    };
    const auto EnableMotor = [](const auto & event, const auto & source, const auto & target, auto &dep) {
        Logger("Action", event, source, target);
        dep.enableMotor();
    };
    const auto DisableMotor = [](const auto & event, const auto & source, const auto & target, auto &dep) {
        Logger("Action", event, source, target);
        dep.disableMotor();
    };

    // clang-format on

    // --------------------------------------------------------------------------
    // State machines
//    struct On {
//        ON();
//
//        static constexpr auto make_transition_table() {
//            // clang-format off
//            return hsm::transition_table(
//                // Source              + Event            [Guard]       / Action                     = Target
//                // +-------------------+------------------+-------------+----------------------------+----------------------+
//                * hsm::state<Idle>     + hsm::event<rotate_step>        / RotateStepMotor            = hsm::state<Busy>,
//                  hsm::state<Idle>     + hsm::event<rotate_continuous>  / RotateContinuousMotor      = hsm::state<BusyContinuous>,
//                  hsm::state<Busy>     + hsm::event<stop>               / StopMotor                  = hsm::state<Idle>,
//                  hsm::state<Busy>     + hsm::event<step_done>          / StepDone                   = hsm::state<Busy>,
//                  hsm::state<Busy>                        [NoMoreSteps] / StopMotor                  = hsm::state<Idle>,
//                  hsm::state<BusyContinuous> + hsm::event<stop>         / StopMotor                  = hsm::state<Idle>,
//                  hsm::state<BusyContinuous> + hsm::event<step_done>    / StepDone                   = hsm::state<BusyContinuous>
//                );
//            // clang-format on
//        }
//    };

    // --------------------------------------------------------------------------
    struct StepperMotorSm {
        ON();

        static constexpr auto make_transition_table() {
            // clang-format off
            return hsm::transition_table(
                // Source              + Event            [Guard]   / Action  = Target
                // +-------------------+------------------+---------+---------+----------------------+
                * hsm::state<Off>     + hsm::event<enable>   / EnableMotor    = hsm::state<On>,
                  hsm::state<On>      + hsm::event<disable>  / DisableMotor   = hsm::state<Off>
                );

            // clang-format on
        }

        static constexpr auto on_unexpected_event() {
            return [](const auto& event, const auto& currentState, const auto& dep) {
                Logger("Unexpected", event, currentState, dep);
            };
        }
    };

}// namespace action
