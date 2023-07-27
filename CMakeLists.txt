cmake_minimum_required(VERSION 3.25)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 CONFIG REQUIRED)
find_package(hsm CONFIG REQUIRED)

project(HsmStepperMotor)

set(CMAKE_CXX_STANDARD 17)

add_executable(motor-action
        main.cpp
        StepperMotorFsmAction.hpp
        StepperMotorFsmEntryExit.hpp
        StepperMotor.hpp
)

target_link_libraries(motor-action PUBLIC hsm::hsm qt::qt)

add_executable(motor-action-gui
        main-gui.cpp
        StepperMotorFsmAction.hpp
        StepperMotor.hpp
        MotorWindow.hpp
        MotorWindow.cpp
)

target_link_libraries(motor-action-gui PUBLIC hsm::hsm qt::qt)
