#include <hsm/hsm.h>
#include "utils.hpp"
#include <iostream>

namespace ee {
    // --------------------------------------------------------------------------
    // States
    struct A1 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
    };

    struct A2 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
    };

    struct A3 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
    };

    struct A4 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
    };

    struct A5 {
        static constexpr auto on_entry() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_ENTRY: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
            };
        }
        static constexpr auto on_exit() {
            return [](const auto &event, const auto &source, const auto &target) {
                std::cout << "      -- ON_EXIT: State Src [" << demangle(source) << "], State Dst[" << demangle(target) << "], Event [" << demangle(event) << "]" << std::endl;
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
    const auto log = [](const auto& event, const auto& source, const auto& target) {
        std::cout << "      -- Event [" << demangle(event) << "], source [" << demangle(source) << "], target [" << demangle(target) << "]" << std::endl;
    };


    // --------------------------------------------------------------------------
    // State machines
    struct EeSm {
        static constexpr auto make_transition_table() {
            // clang-format off
            return boost::hana::make_basic_tuple(
                // Source              + Event            [Guard]   / Action  = Target
                // +-------------------+------------------+---------+---------+----------------------+
//                * hsm::state<A1>       + hsm::event<press>                    = hsm::state<A2>,
//                  hsm::state<A2>       + hsm::event<press>                    = hsm::state<A3>,
//                  hsm::state<A3>       + hsm::event<press>                    = hsm::state<A1>,
                // +-------------------+------------------+---------+---------+----------------------+
                * hsm::state<A4>       + hsm::event<press> [success]/ log     = hsm::state<A5>,
                  hsm::state<A5>       + hsm::event<press>          / log     = hsm::state<A4>
                );

            // clang-format on
        }
    };
}// namespace ee


int main() {
    hsm::sm<ee::EeSm> fsm;

    std::cout << "----- process_event(press) ----- " << std::endl;
    fsm.process_event(ee::press{});

    std::cout << "----- process_event(press) ----- " << std::endl;
    fsm.process_event(ee::press{});

//    std::cout << "----- process_event(press) ----- " << std::endl;
//    fsm.process_event(ee::press{});
//
//    std::cout << "----- process_event(press) ----- " << std::endl;
//    fsm.process_event(ee::press{});
}
