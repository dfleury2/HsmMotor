#pragma once

#include "StepperMotor.hpp"

#include <hsm/hsm.h>

#define ON_ENTRY(className)                                                                                           \
    static constexpr auto on_entry() {                                                                                \
        return [](const auto &event, const auto &source, const auto &target, auto &dep) {                             \
            std::cout << "      -- " #className ": on_entry: " << source.name << " --> " << target.name << std::endl; \
        };                                                                                                            \
    }

#define ON_EXIT(className)                                                                                           \
    static constexpr auto on_exit() {                                                                                \
        return [](const auto &event, const auto &source, const auto &target, auto &dep) {                            \
            std::cout << "      -- " #className ": on_exit: " << source.name << " --> " << target.name << std::endl; \
        };                                                                                                           \
    }

#define ON(className)   \
    ON_ENTRY(className) \
    ON_EXIT(className)

// --------------------------------------------------------------------------
// States
struct Base {
    explicit Base(const char *n) : name(n) {}
    std::string name;
};

struct Init : Base {
    Init() : Base("Init") {}
    ON(Init)
};

struct Off : Base {
    Off() : Base("Off") {}
    ON(Off)
};
struct Idle : Base {
    Idle() : Base("Idle") {}
    ON(Idle)
};
struct Busy : Base {
    Busy() : Base("Busy") {}
    ON(Busy)
};
struct BusyContinuous : Base {
    BusyContinuous() : Base("BusyContinuous") {}
    ON(BusyContinuous)
};

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
// Guards
const auto NoMoreSteps = [](const auto & /* event */, auto & /* source */, const auto & /* target */, const auto &dep) {
    return (dep.getRemainingSteps() <= 0);
};

// --------------------------------------------------------------------------
// Actions
const auto RotateStepMotor = [](const auto &event, const auto & /* source */, const auto & /* target */, auto &dep) {
    dep.rotateStep(event.steps);
};
const auto RotateContinuousMotor = [](const auto &event, const auto & /* source */, const auto & /* target */, auto &dep) {
    dep.rotateContinuous();
};
const auto StopMotor = [](const auto & /* event */, const auto & /* source */, const auto & /* target */, auto &dep) {
    dep.stopMotor();
};
const auto StepDone = [](const auto & /* event */, const auto & /* source */, const auto & /* target */, auto &dep) {
    dep.stepDone();
};
const auto EnableMotor = [](const auto & /* event */, const auto & /* source */, const auto & /* target */, auto &dep) {
    dep.enableMotor();
};
const auto DisableMotor = [](const auto & /* event */, const auto & /* source */, const auto & /* target */, auto &dep) {
    dep.disableMotor();
};

// clang-format on

// --------------------------------------------------------------------------
// State machines
struct On : Base {
    On() : Base("On") {}

    ON(On)

    static constexpr auto make_transition_table() {
        // clang-format off
    return hsm::transition_table(
        // Source              + Event            [Guard]       / Action                     = Target
        // +-------------------+------------------+-------------+----------------------------+----------------------+
        * hsm::state<Idle>     + hsm::event<rotate_step>        / RotateStepMotor            = hsm::state<Busy>,
          hsm::state<Idle>     + hsm::event<rotate_continuous>  / RotateContinuousMotor      = hsm::state<BusyContinuous>,
          hsm::state<Busy>     + hsm::event<stop>               / StopMotor                  = hsm::state<Idle>,
          hsm::state<Busy>     + hsm::event<step_done>          / StepDone                   = hsm::state<Busy>,
          hsm::state<Busy>     + hsm::event<step_done> [NoMoreSteps] / StopMotor             = hsm::state<Idle>,
          hsm::state<BusyContinuous> + hsm::event<stop>         / StopMotor                  = hsm::state<Idle>
        );

        // clang-format on
    }
};

// --------------------------------------------------------------------------
struct StepperMotorSm {
    static constexpr auto make_transition_table() {
        // clang-format off
    return hsm::transition_table(
        // Source              + Event            [Guard]   / Action  = Target
        // +-------------------+------------------+---------+---------+----------------------+
        * hsm::state<Init>     + hsm::event<enable>   / EnableMotor    = hsm::state<On>,
          hsm::state<Off>     + hsm::event<enable>   / EnableMotor    = hsm::state<On>,
          hsm::state<On>      + hsm::event<disable>  / DisableMotor   = hsm::state<Off>
        );

        // clang-format on
    }
};
