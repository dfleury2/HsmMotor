#include <hsm/hsm.h>

#include <iostream>

using namespace boost::hana;

namespace ee {
    struct A0 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << experimental::print(typeid_(source)) << "], State Dst[" << boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst[" << boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
            };
        }
    };
    // --------------------------------------------------------------------------
    // States
    struct A1 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << experimental::print(typeid_(source)) << "], State Dst[" << boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst[" << boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
            };
        }
    };

    struct A2 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst[" << boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << boost::hana::experimental::print(typeid_(source)) << "], State Dst[" << boost::hana::experimental::print(typeid_(target)) << "], Event [" << boost::hana::experimental::print(typeid_(event)) << "]" << std::endl;
            };
        }
    };

    // --------------------------------------------------------------------------
    // Events
    struct press {};

    // --------------------------------------------------------------------------
    // Guard
    const auto success = [](auto /*event*/, auto /*source*/, auto /*target*/) { return true; };

    // --------------------------------------------------------------------------
    // Actions
    const auto log = [](auto event, auto source, auto target) {
        std::cout << boost::hana::experimental::print(typeid_(source)) << " + "
                  << boost::hana::experimental::print(typeid_(event)) << " = "
                  << boost::hana::experimental::print(typeid_(target)) << std::endl;
    };

    // --------------------------------------------------------------------------
    // State machines
    struct InitialEntryExit {
        static constexpr auto make_transition_table() {
            // clang-format off
            return hsm::transition_table(
                // Source              + Event            [Guard]   / Action  = Target
                // +-------------------+------------------+---------+---------+----------------------+
                * hsm::state<A0>                                              = hsm::state<A1>,
                  hsm::state<A1>       + hsm::event<press>          / log     = hsm::state<A2>,
                  hsm::state<A2>       + hsm::event<press>          / log     = hsm::state<A1>
                );

            // clang-format on
        }
    };
}


int main() {
    hsm::sm<ee::InitialEntryExit> fsm;

    std::cout << "----- process_event(press) ----- " << std::endl;
    fsm.process_event(ee::press{});

    std::cout << "----- process_event(press) ----- " << std::endl;
    fsm.process_event(ee::press{});

    std::cout << "----- process_event(press) ----- " << std::endl;
    fsm.process_event(ee::press{});
}
