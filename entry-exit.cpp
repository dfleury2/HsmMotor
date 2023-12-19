#include <simple-hsm/simple-hsm.hpp>

#include <iostream>

#include "utils.hpp"

using namespace boost::hana;

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
struct A0 {
    //        static constexpr auto on_entry() {
    //            return [](const auto &event, const auto &source, const auto &target) {
    //                std::cout << "      -- ON_ENTRY: State Src [" << experimental::print(typeid_(source)) << "], State Dst[" <<
    //                boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event))
    //                << "]" << std::endl;
    //            };
    //        }
    //        static constexpr auto on_exit() {
    //            return [](const auto &event, const auto &source, const auto &target) {
    //                std::cout << "      -- ON_EXIT: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst["
    //                << boost::hana::experimental::print(typeid_(target)) << "], Event [" <<
    //                boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
    //            };
    //        }
    ON(A0);
};
// --------------------------------------------------------------------------
// States
struct A1 {
    //        static constexpr auto on_entry() {
    //            return [](const auto &event, const auto &source, const auto &target) {
    //                std::cout << "      -- ON_ENTRY: State Src [" << experimental::print(typeid_(source)) << "], State Dst[" <<
    //                boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event))
    //                << "]" << std::endl;
    //            };
    //        }
    //        static constexpr auto on_exit() {
    //            return [](const auto &event, const auto &source, const auto &target) {
    //                std::cout << "      -- ON_EXIT: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst["
    //                << boost::hana::experimental::print(typeid_(target)) << "], Event [" <<
    //                boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
    //            };
    //        }
    ON(A1);
};

struct A2 {
    //        static constexpr auto on_entry() {
    //            return [](const auto &event, const auto &source, const auto &target) {
    //                std::cout << "      -- ON_ENTRY: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst["
    //                << boost::hana::experimental::print(typeid_(target)) << "], Event [" <<
    //                boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
    //            };
    //        }
    //        static constexpr auto on_exit() {
    //            return [](const auto &event, const auto &source, const auto &target) {
    //                std::cout << "      -- ON_EXIT: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst["
    //                << boost::hana::experimental::print(typeid_(target)) << "], Event [" <<
    //                boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
    //            };
    //        }
    ON(A2);
};

// --------------------------------------------------------------------------
// Events
struct press {};

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
struct InitialEntryExit {
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
}   // namespace ee

int main()
{
    hsm::sm<ee::InitialEntryExit> fsm;

    for (int i = 0; i < 10; ++i) {
        std::cout << "----- process_event(press) ----- " << std::endl;
        fsm.process_event(ee::press{});
    }
}
