#pragma once

#include "Context.hpp"
#include "Events.hpp"
#include "States.hpp"
#include "Timer.hpp"

#include <bhc/Task.hpp>

#include <boost/asio.hpp>

#include <atomic>
#include <thread>

using TaskMessage =
    std::variant<reset, timeout, calibration_ir_robot_start, calibration_ir_robot::ack_display_gui, calibration_ir_robot::ack_home_pose_robot,
                 calibration_ir_robot::ack_load_pose_robot, calibration_ir_robot::ack_load_confirmation_gui,
                 calibration_ir_robot::ack_move_point_robot, calibration_ir_robot::ack_snapshot>;

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
        m_sequencer_context = SequencerContext{*this};
        sm = {m_sequencer_context};
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
    SequencerContext<Sequencer> m_sequencer_context{*this};
    hsm::sm<StateMachineSequencer, decltype(m_sequencer_context)> sm{m_sequencer_context};

    // Task
    bhc::Task<TaskMessage> m_task;

    // Thread Boost ASIO
    boost::asio::io_context m_ioc;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _work{boost::asio::make_work_guard(m_ioc)};
    std::shared_ptr<Timer> m_timer;

    std::thread m_thread;

    std::atomic_bool m_is_done{false};
};
