#pragma once

#include <vector>

class Sequencer;

// --------------------------------------------------------------------------
// Context
template <typename Sequencer>
class CalibrationContext {
  public:
    explicit CalibrationContext(Sequencer& seq) : sequencer(seq) {}

    std::vector<int> calibration_points = {1, 2, 3, 4, 5};

    void idle() {
        spdlog::info("    --> Idle");
        // TODO
    }

    void startTimer(const std::chrono::steady_clock::duration& d)
    {
        spdlog::info("    --> Start Timer {}s", std::chrono::duration_cast<std::chrono::duration<double>>(d).count());
        sequencer.get().startTimer(d);
    }

    void cancelTimer()
    {
        spdlog::info("    --> Cancel Timer");
        sequencer.get().cancelTimer();
    }

    void error(const std::string& msg) {
        spdlog::error("Error occurred: {}", msg);
        // TODO
    }

    void send_go_loading_pose()
    {
        spdlog::info("    --> Send go to loading pose to robot");
        // TODO
    }

    void send_load_confirmation()
    {
        spdlog::info("    --> Send load confirmation screen to GUI");
        // TODO
    }

    void send_go_to_point()
    {
        spdlog::info("    --> Send go to points {}", calibration_points[0]);
        // TODO
    }

    void remove_point()
    {
        spdlog::info("    --> Remove a point");
        calibration_points.erase(calibration_points.begin());
    }

    void find_bevel()
    {
        spdlog::info("    --> Find Bevel");
        // TODO
    }

    void send_snapshot_request()
    {
        spdlog::info("    --> Send a snapshot request");
        // TODO
    }

    void compute_transform()
    {
        spdlog::info("    --> Compute transform");
        // TODO
    }

    void send_display_calibration_screen()
    {
        spdlog::info("    --> Send DisplayCalibration screen to IHM");
        // TODO
    }

    void send_go_home()
    {
        spdlog::info("    --> Send go to home pose to Robot");
        // TODO
    }

  private:
    std::reference_wrapper<Sequencer> sequencer;
};
