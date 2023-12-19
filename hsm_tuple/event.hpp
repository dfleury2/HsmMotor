#pragma once

#include "forwards.hpp"
#include "pseudo_states.hpp"
#include "transition.hpp"

namespace hsm {

template <class Event> struct event_t {
    inline static constexpr const auto& typeid_{typeid(Event)};
    using type = Event;

    constexpr auto operator+() const
    {
        return details::internal_transition(event_t<Event> {}, noGuard {}, noAction {});
    }

    template <class Guard> constexpr auto operator[](const Guard& guard) const
    {
        return TransitionEG<event_t<Event>, Guard> { guard };
    }

    template <class Action> constexpr auto operator/(const Action& guard) const
    {
        return TransitionEA<event_t<Event>, Action> { guard };
    }
};

template <class Event> const event_t<Event> event {};

struct noneEvent { };

using none = event_t<noneEvent>;
}
