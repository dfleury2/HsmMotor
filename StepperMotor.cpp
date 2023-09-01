#include "StepperMotor.hpp"

#include <iostream>

using namespace std;

void StepperMotor::enableMotor() {
    cout << "StepperMotor::enableMotor" << endl;
    m_isEnable = true;
}

void StepperMotor::disableMotor() {
    cout << "StepperMotor::disableMotor" << endl;
    m_isEnable = false;
}

void StepperMotor::rotateStep(int step) {
    cout << "StepperMotor::rotateStep: " << step << endl;
    m_remaining_steps = step;
}

void StepperMotor::rotateContinuous() {
    cout << "StepperMotor::rotateContinuous" << endl;
}

void StepperMotor::stepDone() {
    cout << "StepperMotor::stepDone: remains " << m_remaining_steps << " step(s)" << endl;
    --m_remaining_steps;
    m_angle += 10;
}

void StepperMotor::stopMotor() {
    cout << "StepperMotor::stopMotor" << endl;
    m_remaining_steps = 0;
}
