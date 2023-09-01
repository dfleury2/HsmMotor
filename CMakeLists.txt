cmake_minimum_required(VERSION 3.25)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 CONFIG REQUIRED)
find_package(hsm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(sml CONFIG REQUIRED)

project(HsmStepperMotor)

set(CMAKE_CXX_STANDARD 17)

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

add_executable(hsm-entry-exit hsm-entry-exit.cpp)
target_link_libraries(hsm-entry-exit PUBLIC hsm::hsm fmt::fmt)

add_executable(sml-entry-exit sml-entry-exit.cpp)
target_link_libraries(sml-entry-exit PUBLIC sml::sml fmt::fmt)

add_executable(entry-exit entry-exit.cpp)
target_link_libraries(entry-exit PUBLIC hsm::hsm)
