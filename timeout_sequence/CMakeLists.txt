add_executable(timeout_sequence_calibration
        Commons/States.hpp
        Calibration_IR_Robot/Context.hpp
        Calibration_IR_Robot/Events.hpp
        Calibration_IR_Robot/Guards.hpp
        Calibration_IR_Robot/States.hpp
        Sequencer/Actions.hpp
        Sequencer/Context.hpp
        Sequencer/Events.hpp
        Sequencer/Sequencer.hpp
        Sequencer/States.hpp
        Sequencer/Timer.hpp
        main.cpp
)

target_link_libraries(timeout_sequence_calibration
    PUBLIC
        simple-hsm::simple-hsm
        fmt::fmt
        Boost::boost
        spdlog::spdlog
        bhc-commons::bhc-commons
)
