#include <hsm/hsm.h>

#include <iostream>

#include "utils.hpp"

#define ON_ENTRY(STATE)                                                                                                                   \
    static constexpr auto on_entry()                                                                                                      \
    {                                                                                                                                     \
        return [](const auto& event, const auto& source, const auto& target) { std::cout << "      -- ENTRY: " << #STATE << std::endl; }; \
    }

#define ON_EXIT(STATE)                                                                                                                   \
    static constexpr auto on_exit()                                                                                                      \
    {                                                                                                                                    \
        return [](const auto& event, const auto& source, const auto& target) { std::cout << "      -- EXIT: " << #STATE << std::endl; }; \
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
struct SubStateEntryExit {
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

    ON(InitialEntryExit);

    static constexpr auto on_unexpected_event()
    {
        return [](auto& event, const auto& state) { log(event, state, state, "unexpected event: "); };
    }
};

struct InitialEntryExit {
    static constexpr auto make_transition_table()
    {
        // clang-format off
            return hsm::transition_table(
                // Source              + Event            [Guard]   / Action  = Target
                // +-------------------+------------------+---------+---------+----------------------+
                * hsm::state<Idle>       + hsm::event<start>          / log     = hsm::state<SubStateEntryExit>
                );

        // clang-format on
    }

    ON(InitialEntryExit);

    static constexpr auto on_unexpected_event()
    {
        return [](auto& event, const auto& state) { log(event, state, state, "unexpected event: "); };
    }
};

}   // namespace ee

int main()
{
    hsm::sm<ee::InitialEntryExit> fsm;

    fsm.process_event(ee::start{});
    for (int i = 0; i < 10; ++i) {
        std::cout << "----- process_event(press) ----- " << std::endl;
        fsm.process_event(ee::press{});
    }
}
