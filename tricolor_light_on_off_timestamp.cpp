#include <fmt/color.h>
#include <hsm/hsm.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using Duration = steady_clock::duration;
using Timestamp = steady_clock::time_point;

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
struct StartStop {
    Timestamp timestamp = steady_clock::now();
};
struct Tick {
    Timestamp timestamp = steady_clock::now();
};

// Guards
const auto Expired = [](const auto& event, auto& /* source */, const auto& /* target */, const auto& dep) {
    return dep.isElapsed(event.timestamp);
};

// Actions
const auto Off_Green = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) { dep.green = nullopt; };
const auto On_Green = [](const auto& event, auto& /* source */, const auto& /* target */, auto& dep) { dep.green = event.timestamp; };

const auto Off_Orange = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) { dep.orange = nullopt; };
const auto On_Orange = [](const auto& event, auto& /* source */, const auto& /* target */, auto& dep) { dep.orange = event.timestamp; };

const auto Off_Red = [](const auto& /* event */, auto& /* source */, const auto& /* target */, auto& dep) { dep.red = nullopt; };
const auto On_Red = [](const auto& event, auto& /* source */, const auto& /* target */, auto& dep) { dep.red = event.timestamp; };

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
        return [](const auto& event, auto& source, auto& target, auto& dep) {
            dep.green = dep.red = nullopt;
            dep.orange = event.timestamp;
        };
    }
    static constexpr auto on_exit()
    {
        return [](const auto& /* event */, auto& source, auto& target, auto& dep) { dep.green = dep.orange = dep.red = nullopt; };
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
    std::optional<Timestamp> green = nullopt;
    std::optional<Timestamp> orange = nullopt;
    std::optional<Timestamp> red = nullopt;

    /// Check if a current light has elapsed
    [[nodiscard]] bool isElapsed(const Timestamp& t) const
    {
        auto lightElapsed = [&t](const auto& l, const auto& d) { return l && t - *l >= d; };

        return lightElapsed(green, DURATION_GREEN) || lightElapsed(orange, DURATION_ORANGE) || lightElapsed(red, DURATION_RED);
    }

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
        }
        cout << flush;
    }
};

int main()
{
    TriColorLight triColorLight;
    hsm::sm<TriColorLightSm, TriColorLight> triColorLightSm{triColorLight};

    for (int i = 0; i < 1000; ++i) {

        if (i % 300 == 0) {
            triColorLightSm.process_event(StartStop{});
        }
        else {
            triColorLightSm.process_event(Tick{});
        }

        triColorLight.display();

        std::this_thread::sleep_for(25ms);
    }
}
