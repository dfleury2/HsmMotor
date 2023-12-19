#pragma once

#include "Events.hpp"

#include "../Commons/States.hpp"
#include "../Calibration_IR_Robot/States.hpp"

#include <simple-hsm/simple-hsm.hpp>

// --------------------------------------------------------------------------
// State Machine
struct StateMachineSequencer {
    static constexpr auto on_entry()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("ENTRY: StateMachineSequencer");
        };
    }
    static constexpr auto on_exit()
    {
        return [](const auto& event, const auto& source, const auto& target, auto& ctx) {
            spdlog::info("EXIT: StateMachineSequencer");
        };
    }

    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
                // Source                                 + Event                         [Guard]  / Action  = Target
                // +--------------------------------------+-----------------------------+---------+----------------------+
              * hsm::state<Idle>                          + hsm::event<calibration_start>         / log_action = hsm::state<Calibration_IR_Robot>,
                hsm::exit<Calibration_IR_Robot, Error>                                            / log_action = hsm::state<Idle>,
                hsm::exit<Calibration_IR_Robot, Idle>                                             / log_action = hsm::state<Idle>
        );

        // clang-format on
    }

    static constexpr auto on_unexpected_event()
    {
        return [](auto& event, const auto& state, const auto& ctx) {
            std::cout << "Unexpected event: " << demangle(event) << " for state: " << demangle(state) << std::endl;
        };
    }
};
