#pragma once

#include "StepperMotor.hpp"

#include <hsm/hsm.h>

//#define ON_ENTRY(className)                                                                                           \
//    static constexpr auto on_entry() {                                                                                \
//        return [](const auto &event, const auto &source, const auto &target, auto &dep) {                             \
//            std::cout << "      -- " #className ": on_entry: " << source.name << " --> " << target.name << std::endl; \
//        };                                                                                                            \
//    }
//
//#define ON_EXIT(className)                                                                                           \
//    static constexpr auto on_exit() {                                                                                \
//        return [](const auto &event, const auto &source, const auto &target, auto &dep) {                            \
//            std::cout << "      -- " #className ": on_exit: " << source.name << " --> " << target.name << std::endl; \
//        };                                                                                                           \
//    }

namespace entryexit {
    // --------------------------------------------------------------------------
    // Events
    struct enable {};
    struct disable {};
    struct stop {};
    struct step_done {};
    struct rotate_step {
        uint32_t steps = 0;
    };
    struct rotate_continuous {};

    // --------------------------------------------------------------------------
    // States
    struct Base {
        explicit Base(const char *n) : name(n) {}
        std::string name;
    };

    struct Off : Base {
        Off() : Base("Off") {}
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target, auto &dep) {
                dep.disableMotor();
                std::cout << "      -- Off : on_entry: " << source.name << " --> " << target.name << std::endl;
            };
        }
    };
    struct Idle {
    };
    struct Busy : Base {
        Busy() : Base("Busy") {}
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target, auto &dep) {
                if constexpr (std::is_same_v<std::decay_t<decltype(event)>, rotate_step>) {
                    dep.rotateStep(event.steps);
                } else if constexpr (std::is_same_v<std::decay_t<decltype(event)>, step_done>) {
                    dep.stepDone();
                }
                std::cout << "      -- Busy : on_entry: " << source.name << " --> " << target.name << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target, auto &dep) {
                if (source.name != target.name) {// Exclude re-entrant events
                    dep.stopMotor();
                    std::cout << "      -- Busy : on_exit: " << source.name << " --> " << target.name << std::endl;
                }
            };
        }
    };
    struct BusyContinuous : Base {
        BusyContinuous() : Base("BusyContinuous") {}
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target, auto &dep) {
                dep.rotateContinuous();
                std::cout << "      -- BusyContinuous : on_entry: " << source.name << " --> " << target.name << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target, auto &dep) {
                dep.stopMotor();
                std::cout << "      -- BusyContinuous : on_exit: " << source.name << " --> " << target.name << std::endl;
            };
        }
    };

    // --------------------------------------------------------------------------
    // Guards
    const auto NoMoreSteps = [](const auto & /* event */, auto & /* source */, const auto & /* target */, const auto &dep) {
        return (dep.getRemainingSteps() <= 0);
    };

    // clang-format on

    // --------------------------------------------------------------------------
    // State machines
    struct On : Base {
        On() : Base("On") {}

        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target, auto &dep) {
                dep.enableMotor();
                std::cout << "      -- On : on_entry: " << source.name << " --> " << target.name << std::endl;
            };
        }

        static constexpr auto make_transition_table() {
            // clang-format off
    return hsm::transition_table(
        // Source              + Event            [Guard]       / Action                     = Target
        // +-------------------+------------------+-------------+----------------------------+----------------------+
        * hsm::state<Idle>     + hsm::event<rotate_step>                                     = hsm::state<Busy>,
          hsm::state<Idle>     + hsm::event<rotate_continuous>                               = hsm::state<BusyContinuous>,
          hsm::state<Busy>     + hsm::event<stop>                                            = hsm::state<Idle>,
          hsm::state<Busy>     + hsm::event<step_done>                                       = hsm::state<Busy>,
          hsm::state<Busy>     + hsm::event<step_done> [NoMoreSteps]                         = hsm::state<Idle>,
          hsm::state<BusyContinuous> + hsm::event<stop>                                      = hsm::state<Idle>
        );

            // clang-format on
        }
    };

    // --------------------------------------------------------------------------
    struct StepperMotorSmEntryExit {
        static constexpr auto make_transition_table() {
            // clang-format off
    return hsm::transition_table(
        // Source              + Event            [Guard]   / Action  = Target
        // +-------------------+------------------+---------+---------+----------------------+
        * hsm::state<Off>     + hsm::event<enable>                    = hsm::state<On>,
          hsm::state<On>      + hsm::event<disable>                   = hsm::state<Off>
        );

            // clang-format on
        }
    };
}// namespace entryexit
