#include <fmt/color.h>
#include <fmt/format.h>
#include <simple-hsm/simple-hsm.hpp>

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using Duration = steady_clock::duration;

namespace {
inline constexpr Duration DURATION_ORANGE = 2s;
inline constexpr Duration DURATION_RED = 5s;
inline constexpr Duration DURATION_GREEN = 8s;
}   // namespace

// States
struct Off {};
struct Orange {};
struct Red {};
struct Green {};

// Events
struct StartStop {};
struct Tick {
    Duration elapsed_time;
};

// Guards
const auto Expired = [](const auto& event, auto& /* source */, const auto& /* target */, auto& dep) {
    dep.remaining -= event.elapsed_time;
    return (dep.remaining < 0s);
};

// Actions
const auto Off_Green = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) { dep.green = false; };
const auto On_Green = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) {
    dep.green = true;
    dep.remaining = DURATION_GREEN;
};

const auto Off_Orange = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) { dep.orange = false; };
const auto On_Orange = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) {
    dep.orange = true;
    dep.remaining = DURATION_ORANGE;
};

const auto Off_Red = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) { dep.red = false; };
const auto On_Red = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) {
    dep.red = true;
    dep.remaining = DURATION_RED;
};

// State Machine
struct On {
    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
            // Source              + Event            [Guard]   / Action  = Target
            // +-------------------+------------------+---------+---------+----------------------+
            * hsm::state<Orange>   + hsm::event<Tick> [Expired] / hsm::chain(Off_Orange, On_Red) = hsm::state<Red>,
              hsm::state<Red>      + hsm::event<Tick> [Expired] / hsm::chain(Off_Red, On_Green)  = hsm::state<Green>,
              hsm::state<Green>    + hsm::event<Tick> [Expired] / hsm::chain(Off_Green, On_Orange)  = hsm::state<Orange>
            );

        // clang-format on
    }

    static constexpr auto on_entry()
    {
        return [](const auto& /* event */, auto& source, auto& target, auto& dep) {
            dep.green = dep.red = false;
            dep.orange = true;
            dep.remaining = DURATION_ORANGE;
        };
    }
    static constexpr auto on_exit()
    {
        return [](const auto& /* event */, auto& source, auto& target, auto& dep) {
            dep.green = dep.orange = dep.red = false;
        };
    }
};

struct TriColorLightSm {
    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
            // Source              + Event            [Guard]   / Action  = Target
            // +-------------------+------------------+---------+---------+----------------------+
            * hsm::state<Off>      + hsm::event<StartStop>                = hsm::state<On>,
              hsm::state<On>       + hsm::event<StartStop>                = hsm::state<Off>
            );

        // clang-format on
    }
};

struct TriColorLight {
    Duration remaining = 0s;
    bool green = false;
    bool orange = false;
    bool red = false;

    void display() const
    {
        using namespace std::chrono;

        cout << "\r\33[2K" << flush;
        if (!green && !orange && !red) {
            cout << "OFF";
        }
        else {
            cout << fmt::format(fg(green ? fmt::color::green : fmt::color::gray), "{:>10}", "GREEN");
            cout << fmt::format(fg(orange ? fmt::color::orange : fmt::color::gray), "{:>10}", "ORANGE");
            cout << fmt::format(fg(red ? fmt::color::red : fmt::color::gray), "{:>10}", "RED");
            cout << fmt::format("{:>18.3f}", duration_cast<duration<double>>(remaining).count());
        }
        cout << flush;
    }
};

int main()
{
    TriColorLight triColorLight;

    hsm::sm<TriColorLightSm, TriColorLight> triColorLightSm{triColorLight};

    auto start_time = steady_clock::now();

    triColorLightSm.process_event(StartStop{});

    for (int i = 0; i < 1000; ++i) {

        if (i && (i % 300 == 0)) {
            triColorLightSm.process_event(StartStop{});
            triColorLight.display();
            continue;
        }

        auto current_time = steady_clock::now();

        triColorLightSm.process_event(Tick{.elapsed_time = current_time - start_time});

        triColorLight.display();

        start_time = current_time;

        std::this_thread::sleep_for(25ms);
    }
}
