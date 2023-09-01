#include <boost/sml.hpp>
#include <fmt/format.h>

#include <iostream>

namespace sml = boost::sml;

namespace ee {
    // --------------------------------------------------------------------------
    // States
    struct A1 {};
    struct A2 {};
    struct A3 {};
    struct A4 {};
    struct A5 {};

    // --------------------------------------------------------------------------
    // Events
    struct press {};
    struct depress {};

    // --------------------------------------------------------------------------
    // State machines
    struct EeSm {
        auto operator()() const {
            // clang-format off
            return boost::sml::make_transition_table(
                // Source              + Event            [Guard]   / Action  = Target
                // +-------------------+------------------+---------+---------+----------------------+
                * sml::state<A1>       + sml::event<press>                    = sml::state<A2>,
                  sml::state<A2>       + sml::event<depress>                  = sml::state<A3>,
                  sml::state<A3>       + sml::event<press>                    = sml::state<A1>,
                // +-------------------+------------------+---------+---------+----------------------+
                * sml::state<A4>       + sml::event<press>                    = sml::state<A5>,
                  sml::state<A5>       + sml::event<press>                    = sml::state<A4>
            );

            // clang-format on
        }
    };

    struct my_logger {
        template<class SM, class TEvent>
        void log_process_event(const TEvent &) {
            std::cout << fmt::format("      -- [{}] ON_EVENT Event: [{}]", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TEvent>()) << std::endl;
        }

        template<class SM, class TGuard, class TEvent>
        void log_guard(const TGuard &, const TEvent &, bool result) {
            std::cout << fmt::format("      -- [{}] ON_GUARD Guard: [{}], Event: [{}], Result: [{}]", sml::aux::get_type_name<TGuard>(),
                                     sml::aux::get_type_name<TEvent>(), (result ? "Ok" : "Rejected"))
                      << std::endl;
        }

        template<class SM, class TAction, class TEvent>
        void log_action(const TAction &, const TEvent &) {
            std::cout << fmt::format("      -- [{}] ON_ACTION Action: [{}], Event: [{}]", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TAction>(),
                                     sml::aux::get_type_name<TEvent>())
                      << std::endl;
        }

        template<class SM, class TSrcState, class TDstState>
        void log_state_change(const TSrcState &src, const TDstState &dst) {
            std::cout << fmt::format("      -- [{}] ON_STATE_CHANGE State Src: [{}], State Dst: [{}]", sml::aux::get_type_name<SM>(), src.c_str(), dst.c_str()) << std::endl;
        }
    };
}// namespace ee


int main() {
    ee::my_logger logger;
    sml::sm<ee::EeSm, sml::logger<ee::my_logger>> fsm{logger};

    std::cout << "----- process_event(press) ----- " << std::endl;
    fsm.process_event(ee::press{});

    std::cout << "----- process_event(press) ----- " << std::endl;
    fsm.process_event(ee::press{});

    std::cout << "----- process_event(depress) ----- " << std::endl;
    fsm.process_event(ee::depress{});

    //    std::cout << "----- process_event(press) ----- " << std::endl;
    //    fsm.process_event(ee::press{});
    //
    //    std::cout << "----- process_event(press) ----- " << std::endl;
    //    fsm.process_event(ee::press{});
}
