#pragma once

#include "../Calibration_IR_Robot/Context.hpp"

#include <spdlog/spdlog.h>

// --------------------------------------------------------------------------
// Context
template <typename Sequencer>
class SequencerContext : public calibration_ir_robot::CalibrationIrRobotContext<Sequencer> {
  public:
    explicit SequencerContext(Sequencer& seq) : calibration_ir_robot::CalibrationIrRobotContext<Sequencer>{seq}, m_sequencer(seq) {}

    void idle()
    {
        spdlog::info("    --> Idle");
        // TODO
    }

    void start_timer(const std::chrono::steady_clock::duration& d)
    {
        spdlog::info("    --> Start Timer {}s", std::chrono::duration_cast<std::chrono::duration<double>>(d).count());
        m_sequencer.get().startTimer(d);
    }

    void cancel_timer()
    {
        spdlog::info("    --> Cancel Timer");
        m_sequencer.get().cancelTimer();
    }

    void error(const std::string& msg)
    {
        spdlog::error("Error occurred: {}", msg);
        // TODO
    }

  private:
    std::reference_wrapper<Sequencer> m_sequencer;
};
