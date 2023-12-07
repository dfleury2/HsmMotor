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
        : m_timer(ioc), m_duration(d), m_cb(std::forward<Callback>(cb))
    {
    }

    void start()
    {
        m_timer.expires_after(m_duration);

        m_timer.async_wait([me = shared_from_this()](const boost::system::error_code& ec) {
            if (!ec) {
                me->m_cb();
            }
        });
    }

    void stop() { m_timer.cancel(); }

  private:
    boost::asio::steady_timer m_timer;
    std::chrono::steady_clock::duration m_duration;
    timer_cb m_cb;
};

// --------------------------------------------------------------------------
template <typename Callback>
std::shared_ptr<Timer> after(boost::asio::io_context& ioc, const duration& d, Callback&& cb)
{
    auto t = std::make_shared<Timer>(ioc, d, std::forward<Callback>(cb));
    t->start();
    return t;
}

// --------------------------------------------------------------------------
template <typename Callback>
std::shared_ptr<Timer> after(boost::asio::io_context& ioc, double seconds, Callback&& cb)
{
    return after(ioc, std::chrono::milliseconds((int)(seconds * 1000)), std::forward<Callback>(cb));
}
