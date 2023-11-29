#include <doctest/doctest.h>

#include <hsm/hsm.h>

namespace {

// States
struct S1
{};
struct S2
{};
struct S3
{};
struct S4
{};
struct S5
{};
struct S6
{};
struct S7
{};

// Events
struct e1
{};
struct e2
{};
struct e3
{};
struct e4
{};
struct e5
{};
struct e6
{};

// Guards
const auto trueGuard = [](auto /*event*/, auto /*source*/, auto /*target*/) { return true; };
const auto falseGuard = [](auto /*event*/, auto /*source*/, auto /*target*/) { return false; };

struct SubState
{
    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
            // Region 0    
            * hsm::state<S1> + hsm::event<e1> [trueGuard] =  hsm::state<S2>
        );
        // clang-format on
    }
};

struct MainState
{
    static constexpr auto make_transition_table()
    {
        // clang-format off
        return hsm::transition_table(
            // Region 0    
            * hsm::state<S1> + hsm::event<e1> [trueGuard]  = hsm::state<S2>,
              hsm::state<S1> + hsm::event<e2> [trueGuard]  = hsm::state<S4>,
              // Guard tests
              hsm::state<S1> + hsm::event<e3> [falseGuard] = hsm::state<S2>,
              hsm::state<S1> + hsm::event<e4> [trueGuard]  = hsm::state<S2>,
              hsm::state<S1> + hsm::event<e5> [falseGuard] = hsm::state<S2>,
              hsm::state<S1> + hsm::event<e6> [trueGuard]  = hsm::state<S6>,
              hsm::state<S6>                  [falseGuard] = hsm::state<S1>,
              hsm::state<S2> + hsm::event<e1> [trueGuard]  = hsm::state<SubState>,
              hsm::state<S4>                  [trueGuard]  = hsm::state<S1>,
            // Region 1
            * hsm::state<S3> + hsm::event<e1> [trueGuard]  = hsm::state<S4>,
              hsm::state<S3> + hsm::event<e2> [trueGuard]  = hsm::state<S5>,
              // Guard tests            
              hsm::state<S3> + hsm::event<e3> [falseGuard] = hsm::state<S2>,
              hsm::state<S3> + hsm::event<e4> [falseGuard] = hsm::state<S2>,
              hsm::state<S3> + hsm::event<e5> [trueGuard]  = hsm::state<S2>,
              hsm::state<S3> + hsm::event<e6> [trueGuard]  = hsm::state<S7>,
              hsm::state<S7>                  [trueGuard]  = hsm::state<S1>,
              hsm::state<S5>                  [trueGuard]  = hsm::state<S1>
        );
        // clang-format on
    }
};
}   // namespace

TEST_CASE("Orthogonal Regions")
{
    hsm::sm<MainState> sm;

    SUBCASE("should_start_in_region_initial_states")
    {
        REQUIRE(sm.is(0, hsm::state<MainState>, hsm::state<S1>));
        REQUIRE(sm.is(1, hsm::state<MainState>, hsm::state<S3>));
    }
    SUBCASE("should_transit_in_all_regions")
    {
        sm.process_event(e1{});
        REQUIRE(sm.is(0, hsm::state<MainState>, hsm::state<S2>));
        REQUIRE(sm.is(1, hsm::state<MainState>, hsm::state<S4>));
    }

    SUBCASE("should_anonymous_transit_in_all_regions")
    {
        sm.process_event(e2{});
        REQUIRE(sm.is(0, hsm::state<MainState>, hsm::state<S1>));
        REQUIRE(sm.is(1, hsm::state<MainState>, hsm::state<S1>));
    }

    SUBCASE("should_guard_in_all_regions")
    {
        sm.process_event(e3{});
        REQUIRE(sm.is(0, hsm::state<MainState>, hsm::state<S1>));
        REQUIRE(sm.is(1, hsm::state<MainState>, hsm::state<S3>));
    }

    SUBCASE("should_guard_in_a_single_regions")
    {
        sm.process_event(e4{});
        REQUIRE(sm.is(0, hsm::state<MainState>, hsm::state<S2>));
        REQUIRE(sm.is(1, hsm::state<MainState>, hsm::state<S3>));
    }

    SUBCASE("should_guard_in_a_single_regions_inverse")
    {
        sm.process_event(e5{});
        REQUIRE(sm.is(0, hsm::state<MainState>, hsm::state<S1>));
        REQUIRE(sm.is(1, hsm::state<MainState>, hsm::state<S2>));
    }

    SUBCASE("should_anonymous_guard_in_a_single_regions")
    {
        sm.process_event(e6{});
        REQUIRE(sm.is(0, hsm::state<MainState>, hsm::state<S6>));
        REQUIRE(sm.is(1, hsm::state<MainState>, hsm::state<S1>));
    }
}
