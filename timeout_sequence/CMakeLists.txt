add_executable(timeout_sequence_calibration
        main.cpp
        Calibration_IR_Robot/Context.hpp
        Calibration_IR_Robot/Events.hpp
)

target_link_libraries(timeout_sequence_calibration
    PUBLIC
        hsm::hsm
        fmt::fmt
        Boost::boost
        bhc-commons::bhc-commons
)
