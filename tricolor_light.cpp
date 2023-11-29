#include <fmt/color.h>
#include <fmt/format.h>
#include <hsm/hsm.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

using Duration = std::chrono::steady_clock::duration;

// States
struct Off
{
};

struct Orange
{
    inline static constexpr Duration DURATION = 2s;
    Duration remaining = Orange::DURATION;

    static constexpr auto on_entry()
    {
        return [](const auto& /* event */, auto& source, auto& target, auto& dep) {
            target.remaining = DURATION;
            dep.orange = true;
            dep.count = 0;
        };
    }

    static constexpr auto on_exit()
    {
        return [](auto /* event */, auto /*source*/, auto /*target*/, auto& dep) { dep.orange = false; };
    }
};

struct Red
{
    inline static constexpr Duration DURATION = 5s;
    Duration remaining = Red::DURATION;

    static constexpr auto on_entry()
    {
        return [](const auto& /* event */, auto& source, auto& target, auto& dep) {
            target.remaining = DURATION;
            dep.red = true;
            dep.count = 0;
        };
    }
    static constexpr auto on_exit()
    {
        return [](auto /* event */, auto /*source*/, auto /*target*/, auto& dep) { dep.red = false; };
    }
};

struct Green
{
    inline static constexpr Duration DURATION = 8s;
    Duration remaining = DURATION;

    static constexpr auto on_entry()
    {
        return [](const auto& /* event */, auto& source, auto& target, auto& dep) {
            target.remaining = Green::DURATION;
            dep.green = true;
            dep.count = 0;
        };
    }
    static constexpr auto on_exit()
    {
        return [](auto /* event */, auto /*source*/, auto /*target*/, auto& dep) { dep.green = false; };
    }
};

// Events
struct StartStop
{
};

struct Tick
{
    std::chrono::steady_clock::duration elapsed_time;
};

// Guards
const auto Expired = [](const auto& event, auto& source, const auto& /* target */, auto& dep) {
    source.remaining -= event.elapsed_time;
    return (source.remaining < 0s);
};

// State Machine
struct TriColorLightSm
{
    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
            // Source              + Event            [Guard]   / Action  = Target
            // +-------------------+------------------+---------+---------+----------------------+
            * hsm::state<Off>      + hsm::event<StartStop>                = hsm::state<Orange>,
              hsm::state<Orange>   + hsm::event<Tick> [Expired]           = hsm::state<Red>,
              hsm::state<Red>      + hsm::event<Tick> [Expired]           = hsm::state<Green>,
              hsm::state<Green>    + hsm::event<Tick> [Expired]           = hsm::state<Orange>
            );

        // clang-format on
    }
};

int main()
{
    struct TriColorLight
    {
        bool green = false;
        bool orange = false;
        bool red = false;
        int count = 0;
    } triColorLight;

    hsm::sm<TriColorLightSm, TriColorLight> triColorLightSm{triColorLight};

    auto start_time = std::chrono::steady_clock::now();
    auto first_start_time = start_time;

    triColorLightSm.process_event(StartStop{});

    for (;;) {
        auto current_time = std::chrono::steady_clock::now();

        cout << "\r\33[2K" << flush;
        cout << fmt::format(fg(triColorLight.green ? fmt::color::green : fmt::color::gray), "{:>10}", "GREEN");
        cout << fmt::format(fg(triColorLight.orange ? fmt::color::orange : fmt::color::gray), "{:>10}", "ORANGE");
        cout << fmt::format(fg(triColorLight.red ? fmt::color::red : fmt::color::gray), "{:>10}", "RED");
        cout << " - "
             << fmt::format("{:>18.3f}", std::chrono::duration_cast<std::chrono::duration<double>>(current_time - first_start_time).count())
             << " " << string(triColorLight.count / 10, '.') << flush;

        if (triColorLight.count == 0)
            first_start_time = current_time;

        ++triColorLight.count;
        triColorLightSm.process_event(Tick{.elapsed_time = current_time - start_time});

        start_time = current_time;

        std::this_thread::sleep_for(25ms);
    }
}
