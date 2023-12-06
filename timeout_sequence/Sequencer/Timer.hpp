#pragma once

#include <boost/asio.hpp>

#include <chrono>
#include <functional>
#include <memory>

using duration = std::chrono::steady_clock::duration;
using timer_cb = std::function<void()>;

// --------------------------------------------------------------------------
class Timer : public std::enable_shared_from_this<Timer> {
  public:
    template <typename Callback>
    Timer(boost::asio::io_context& ioc, const std::chrono::steady_clock::duration& d, Callback&& cb)
        : _timer(ioc), _duration(d), _cb(std::forward<Callback>(cb))
    {}

    void start() { register_timer(); rearm(); }
    void stop() { _timer.cancel(); }

  private:
    void register_timer()
    {
        bool found = false;
        for (auto& t : Timers) {
            if (t.get() == this) {
                found = true;
                break;
            }
        }
        if (!found) {
            Timers.push_back(shared_from_this());
        }
    }

    void rearm()
    {
        _timer.expires_after(_duration);

        _timer.async_wait([me = shared_from_this()](const boost::system::error_code& ec) {
            if (!ec) {
                me->_cb();
            }
        });
    }

  private:
    boost::asio::steady_timer _timer;
    std::chrono::steady_clock::duration _duration;
    timer_cb _cb;

    inline static std::vector<std::shared_ptr<Timer>> Timers;
};

// --------------------------------------------------------------------------
template <typename Callback>
std::shared_ptr<Timer> after(boost::asio::io_context& ioc, const duration& d, Callback&& cb)
{
    auto t = std::make_shared<Timer>(ioc, d, std::forward<Callback>(cb));
    t->start();
    return t;
}

template <typename Callback>
std::shared_ptr<Timer> after(boost::asio::io_context& ioc, double seconds, Callback&& cb)
{
    return after(ioc, std::chrono::milliseconds((int)(seconds * 1000)), std::forward<Callback>(cb));
}
