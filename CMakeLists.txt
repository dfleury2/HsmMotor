cmake_minimum_required(VERSION 3.25)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED)
find_package(hsm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(sml CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

project(HsmStepperMotor)

set(CMAKE_CXX_STANDARD 20)

add_executable(generate_plantuml generate_plantuml.cpp)

add_executable(motor-action
        main.cpp
        StepperMotorFsmAction.hpp
        StepperMotorFsmEntryExit.hpp
        StepperMotor.hpp
        StepperMotor.cpp
)

target_link_libraries(motor-action PUBLIC hsm::hsm qt::qt)

add_executable(motor-action-gui
        main-gui.cpp
        StepperMotorFsmAction.hpp
        StepperMotor.hpp
        StepperMotor.cpp
        MotorWindow.hpp
        MotorWindow.cpp
)

target_link_libraries(motor-action-gui PUBLIC hsm::hsm qt::qt fmt::fmt)

if(WIN32)
    target_link_libraries(motor-action-gui
        PUBLIC
            DWrite.lib
            Shcore.lib
            D3d9.lib
    )
endif()


add_executable(hsm-entry-exit hsm-entry-exit.cpp)
target_link_libraries(hsm-entry-exit PUBLIC hsm::hsm fmt::fmt)

add_executable(sml-entry-exit sml-entry-exit.cpp)
target_link_libraries(sml-entry-exit PUBLIC sml::sml fmt::fmt)

add_executable(entry-exit entry-exit.cpp)
target_link_libraries(entry-exit PUBLIC hsm::hsm)


add_executable(sequence_calibration sequence_calibration.cpp)
target_link_libraries(sequence_calibration PUBLIC hsm::hsm fmt::fmt Boost::boost)


add_executable(tricolor_light tricolor_light.cpp)
target_link_libraries(tricolor_light PUBLIC hsm::hsm fmt::fmt)

add_executable(tricolor_light_on_off tricolor_light_on_off.cpp)
target_link_libraries(tricolor_light_on_off PUBLIC hsm::hsm fmt::fmt)

add_executable(tricolor_light_on_off_timestamp tricolor_light_on_off_timestamp.cpp)
target_link_libraries(tricolor_light_on_off_timestamp PUBLIC hsm::hsm fmt::fmt)

enable_testing()

add_subdirectory(hsm_example)
add_subdirectory(hsm_tests)
add_subdirectory(hsm_gui)
