#include <simple-hsm/simple-hsm.hpp>

#include <iostream>

#include "utils.hpp"

#define ON_ENTRY(STATE)                                                                                                            \
    static constexpr auto on_entry()                                                                                               \
    {                                                                                                                              \
        return [](const auto& event, const auto& source, const auto& target) {                                                     \
            std::cout << "      -- ENTRY: " << #STATE << " (source=" << demangle(source) << ", target:" << demangle(target) << ")" \
                      << std::endl;                                                                                                \
        };                                                                                                                         \
    }

#define ON_EXIT(STATE)                                                                                                            \
    static constexpr auto on_exit()                                                                                               \
    {                                                                                                                             \
        return [](const auto& event, const auto& source, const auto& target) {                                                    \
            std::cout << "      -- EXIT: " << #STATE << " (source=" << demangle(source) << ", target:" << demangle(target) << ")" \
                      << std::endl;                                                                                               \
        };                                                                                                                        \
    }

#define ON(STATE)   \
    ON_ENTRY(STATE) \
    ON_EXIT(STATE)

namespace ee {
struct Idle {
    ON(Idle);
};

struct A0 {
    ON(A0);
};
// --------------------------------------------------------------------------
// States
struct A1 {
    ON(A1);
};

struct A2 {
    ON(A2);
};

// --------------------------------------------------------------------------
// Events
struct press {};
struct start {};
struct stop {};

// --------------------------------------------------------------------------
// Guard
const auto success = [](auto /*event*/, auto /*source*/, auto /*target*/) { return true; };

// --------------------------------------------------------------------------
// Actions
const auto log = [](auto event, auto source, auto target, const char* msg = "") {
    std::cout << msg << demangle(source) << " + " << demangle(event) << " = " << demangle(target) << std::endl;
};

// --------------------------------------------------------------------------
// State machines
struct SubState {
    static constexpr auto make_transition_table()
    {
        // clang-format off
            return hsm::transition_table(
                // Source              + Event            [Guard]   / Action  = Target
                // +-------------------+------------------+---------+---------+----------------------+
                * hsm::state<A0>       + hsm::event<press>          / log     = hsm::state<A1>,
                  hsm::state<A1>       + hsm::event<press>          / log     = hsm::state<A2>,
                  hsm::state<A2>       + hsm::event<press>          / log     = hsm::state<A0>
                );

        // clang-format on
    }

    ON(SubState);

    static constexpr auto on_unexpected_event()
    {
        return [](auto& event, const auto& state) { log(event, state, state, "unexpected event: "); };
    }
};

struct Initial {

    static constexpr auto make_transition_table()
    {
        // clang-format off
            return hsm::transition_table(
                // Source              + Event            [Guard]   / Action  = Target
                // +-------------------+------------------+---------+---------+----------------------+
                * hsm::state<Idle>       + hsm::event<start>          / log     = hsm::state<SubState>,
                  hsm::state<SubState>   + hsm::event<stop>           / log     = hsm::state<Idle>
                );

        // clang-format on
    }

    ON(Initial);

    static constexpr auto on_unexpected_event()
    {
        return [](auto& event, const auto& state) { log(event, state, state, "unexpected event: "); };
    }
};

}   // namespace ee

int main()
{
    std::cout << "------------------------------------------- Initial" << std::endl;
    {
        hsm::sm<ee::Initial> fsm;

        std::cout << "----- process_event(start) ----- " << std::endl;
        fsm.process_event(ee::start{});

        for (int i = 0; i < 4; ++i) {
            std::cout << "----- process_event(press) ----- " << std::endl;
            fsm.process_event(ee::press{});
        }

        fsm.process_event(ee::stop{});
    }

    std::cout << "------------------------------------------- SubState" << std::endl;
    {
        hsm::sm<ee::SubState> fsm;

        for (int i = 0; i < 4; ++i) {
            std::cout << "----- process_event(press) ----- " << std::endl;
            fsm.process_event(ee::press{});
        }
    }
}
