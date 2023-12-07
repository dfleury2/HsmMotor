#pragma once

#include "Events.hpp"
#include "States.hpp"
#include "Timer.hpp"

#include "../Calibration_IR_Robot/Context.hpp"

#include <bhc/Task.hpp>

#include <boost/asio.hpp>

#include <atomic>
#include <thread>

using TaskMessage = std::variant<reset, timeout, calibration_start, ack_display_ihm, ack_home_pose_robot, ack_load_pose_robot,
                                 ack_load_confirmation_ihm, ack_move_point_robot, ack_snapshot>;

// --------------------------------------------------------------------------
class Sequencer {
  public:
    Sequencer()
    {
        m_task.start("sequencer", [this](TaskMessage&& msg) {
            std::visit([this](auto&& event) { handle(std::forward<decltype(event)>(event)); }, std::move(msg));
        });

        m_thread = std::thread([this] {
            spdlog::info("Starting ASIO thread");
            while (!isDone()) {
                try {
                    spdlog::info("Starting ASIO main loop");
                    m_ioc.run();
                }
                catch (const std::exception& ex) {
                    spdlog::error("Asio error: {}", ex.what());
                }
            }
        });
    }

    ~Sequencer()
    {
        m_task.stop();

        m_is_done = true;

        m_ioc.stop();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    [[nodiscard]] bool isDone() const noexcept { return m_is_done; }

    void put(TaskMessage&& msg) { m_task.put(std::move(msg)); }

    void reset()
    {
        calibrationContext = CalibrationContext{*this};
        sm = {calibrationContext};
    }

    void startTimer(const std::chrono::steady_clock::duration& d)
    {
        cancelTimer();

        m_timer = after(m_ioc, d, [this]() { m_task.put(timeout{}); });
    }

    void cancelTimer()
    {
        if (m_timer) {
            m_timer->stop();
        }
    }

    template <typename Event>
    void handle(Event&& event)
    {
        sm.process_event(std::forward<Event>(event));
    }

    void handle(::reset&&) { this->reset(); }

  private:
    CalibrationContext<Sequencer> calibrationContext{*this};
    hsm::sm<StateMachineSequencer, CalibrationContext<Sequencer>> sm{calibrationContext};

    // Task
    bhc::Task<TaskMessage> m_task;

    // Thread Boost ASIO
    boost::asio::io_context m_ioc;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _work{boost::asio::make_work_guard(m_ioc)};
    std::shared_ptr<Timer> m_timer;

    std::thread m_thread;

    std::atomic_bool m_is_done{false};
};
