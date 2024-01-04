// #include <simple-hsm/simple-hsm.hpp>

#include "log.hpp"
#include "state.hpp"
#include "transition_dsl.h"
#include "transition_table.hpp"

#include "../utils.hpp"

#include <iostream>
#include <tuple>
#include <unordered_set>

using namespace std;

namespace ee {

// States
struct S1 {};
struct S2 {};
struct Exit {};

// Events
struct e1 {};
struct e2 {};
struct e3 {};
struct e4 {};

struct SubState {

    static constexpr auto make_transition_table()
    {
        // clang-format off
         return std::make_tuple(
             * hsm::state<S1> + hsm::event<e1> = hsm::state<S1>
         );
        // clang-format on
    }
};

struct SubState2 {
    static constexpr auto make_transition_table()
    {
        // clang-format off
         return std::make_tuple(
             * hsm::state<S2> + hsm::event<e1> = hsm::state<S2>
         );
        // clang-format on
    }
};

struct SubState3 {
    static constexpr auto on_exit()
    {
        return [](auto event, auto source, auto target) { std::cout << "on_exit" << std::endl; };
    }

    static constexpr auto make_transition_table()
    {
        // clang-format off
         return std::make_tuple(
             * hsm::state<S1> + hsm::event<e4>  = hsm::state<Exit>
         );
        // clang-format on
    }
};

struct MainState {
    static constexpr auto make_transition_table()
    {
        // clang-format off
        return std::make_tuple(
          * hsm::state<S1>             + hsm::event<e1> = hsm::state<S2>,
            hsm::state<S2>             + hsm::event<e1> / hsm::log = hsm::state<S1>,
            hsm::state<S1>             + hsm::event<e2> = hsm::state<SubState>,
            hsm::state<S1>             + hsm::event<e3> / hsm::log = hsm::state<SubState2>,
            hsm::state<S1>             + hsm::event<e4> = hsm::state<SubState3>,
            hsm::state<SubState>       + hsm::event<e2> = hsm::state<S1>,
            hsm::exit<SubState3, Exit>                  / hsm::log = hsm::state<S1>
        );
        // clang-format on
    }
};
}   // namespace ee

// constexpr auto is_initial_state = [](auto State) {
//     if constexpr (std::is_base_of_v<hsm::InitialPseudoState, typename decltype(State)::type>) {
//         return true;
//     }
//     return false;
// };

template <typename State>
constexpr bool is_initial_state(State)
{
    return std::is_base_of_v<hsm::InitialPseudoState, typename State::type>;
};

std::unordered_set<std::string> Events;

template <typename Transition>
void called(Transition t)
{
    std::cout << demangle(t) << std::endl;

    std::cout << "  -  " << demangle(t.source()) << std::endl;
    if constexpr (is_initial_state(t.source())) {
        cout << "     - is an initial state" << std::endl;
        cout << "      - to  " << demangle<decltype(std::declval<typename decltype(t.source())::type>().get_state())>() << std::endl;
    }

    std::cout << "  -  " << demangle(t.event()) << std::endl;
    cout << "      - to  " << demangle<decltype(std::declval<typename decltype(t.event())::type>())>() << std::endl;
    cout << "        hash=  " << typeid(decltype(std::declval<typename decltype(t.event())::type>())).name() << std::endl;

    Events.insert(typeid(decltype(std::declval<typename decltype(t.event())::type>())).name());

    std::cout << "  -  " << demangle(t.action()) << std::endl;
    std::cout << "  -  " << demangle(t.guard()) << std::endl;
    std::cout << "  -  " << demangle(t.target()) << std::endl;
    cout << "      - to  " << demangle<decltype(std::declval<typename decltype(t.target())::type>())>() << std::endl;
    cout << "        hash=  " << typeid(decltype(std::declval<typename decltype(t.target())::type>())).name() << std::endl;
}

template <typename T>
auto func(T)
{
    //return decltype(std::declval<typename decltype(t)::type>()){};
    return decltype(std::declval<typename T::type>()){};
}

template <typename... Args>
auto funcs(Args... as)
{
    return std::make_tuple(func(as.event())...);
}

template <typename T, typename Tuple>
struct my_has_type;

template <typename T>
struct my_has_type<T, std::tuple<>> : std::false_type {};

template <typename T, typename U, typename... Ts>
struct my_has_type<T, std::tuple<U, Ts...>> : my_has_type<T, std::tuple<Ts...>> {};

template <typename T, typename... Ts>
struct my_has_type<T, std::tuple<T, Ts...>> : std::true_type {};

template <typename T, typename Tuple>
using my_has_type_t = typename my_has_type<T, Tuple>::type;

template <class RootState>
struct Automate {
    using Transitions = decltype(RootState::make_transition_table());

    Automate()  = default;

    static constexpr auto extract_events(Transitions t) {
        return std::apply([](const auto&... ts) { return funcs(ts...); }, t);
    }

    Transitions m_transitions = RootState::make_transition_table();

    using Events = decltype(extract_events(m_transitions));

    template <typename Event>
    void process_event(Event)
    {
        static_assert(my_has_type_t<Event, Events>::value, "Event not found");
    }

};


template<class T> struct is_tuple : std::false_type {};
template<class... Ts> struct is_tuple<std::tuple<Ts...>> : std::true_type {};

template <typename... Ts>
inline constexpr bool is_tuple_v = is_tuple<Ts...>::value;


//template < typename T, typename ...Rest >
//constexpr auto flatten( std::tuple< T, Rest... > )
//{
//    if constexpr ( is_tuple_v<T> )
//    {
//        if constexpr ( sizeof...( Rest ) > 0 ) {
//            using remaining = decltype( flatten( std::tuple< Rest... >{} ) );
//            return std::tuple_cat(flatten(std::tuple< T >{}), remaining{} );
//        }
//        else {
//            return std::make_tuple(flatten(T{}));
//        }
//    }
//    else
//    {
//        if constexpr ( sizeof...( Rest ) > 0 )
//        {
//            using remaining = decltype( flatten( std::tuple< Rest... >{} ) );
//            return std::tuple_cat( std::tuple< T >{}, remaining{} );
//        }
//        else
//        {
//            return std::tuple< T >{};
//        }
//    }
// }

template <typename T>
constexpr auto explode(T, std::size_t)
{
    return std::tuple<T>{};
}

template <typename T, std::size_t I = std::tuple_size<T>{}>
constexpr auto explode(T, int);

template <typename T, std::size_t... Is>
constexpr auto explode(T, std::index_sequence<Is...>)
{
    return std::tuple_cat(explode(std::get<Is>(T{}), 0)...);
}

template <typename T, std::size_t I>
constexpr auto explode(T, int)
{
    return explode(T{}, std::make_index_sequence<I>{});
}

template <typename T, std::size_t... Is>
constexpr auto flatten(T, std::index_sequence<Is...>)
{
    return std::tuple_cat(explode(std::get<Is>(T{}), 0)...);
}

template <typename T>
constexpr auto flatten(T)
{
    return flatten(T{}, std::make_index_sequence<std::tuple_size<T>{}>{});
}


int main()
{
    std::tuple<ee::e1, ee::e2, std::tuple<ee::S1, ee::e4>, ee::e3, ee::e4> es;

    std::tuple<ee::S1, ee::S2> states1;
    std::tuple<ee::S2, decltype(es)> states2;

    constexpr auto s = std::tuple_cat(states1, states2);

    constexpr auto f = flatten(s);

    //    hsm::sm<MainState> sm;

    auto transitions = ee::MainState::make_transition_table();

        std::apply([](const auto&... ts) { (called(ts), ...); }, transitions);

        for(auto&& e :  Events) {
            cout << "Event: " << e << endl;
        }

//    auto T = std::apply([](const auto&... ts) { return funcs(ts...); }, transitions);

    Automate<ee::MainState> sm;
    sm.process_event(ee::e1{});
}
