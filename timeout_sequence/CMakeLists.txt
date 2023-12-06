add_executable(timeout_sequence_calibration timeout_sequence_calibration.cpp)

target_link_libraries(timeout_sequence_calibration
    PUBLIC
        hsm::hsm
        fmt::fmt
        Boost::boost
        bhc-commons::bhc-commons
)
