cmake_minimum_required(VERSION 3.25)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

find_package(hsm CONFIG REQUIRED)

project(HsmStepperMotor)

set(CMAKE_CXX_STANDARD 17)

add_executable(motor_action main.cpp StepperMotorFsmAction.hpp StepperMotorFsmEntryExit.hpp StepperMotor.hpp)

target_link_libraries(motor_action PUBLIC hsm::hsm)
