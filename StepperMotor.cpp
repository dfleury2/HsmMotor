#include "StepperMotor.hpp"

#include <iostream>

using namespace std;

void StepperMotor::enableMotor() {
    cout << "+enableMotor" << endl;
    m_isEnable = true;
}

void StepperMotor::disableMotor() {
    cout << "+disableMotor" << endl;
    m_isEnable = false;
}

void StepperMotor::rotateStep(int step) {
    cout << "+rotateStep: " << step << endl;
    m_remaining_steps = step;
}

void StepperMotor::rotateContinuous() {
    cout << "+rotateContinuous" << endl;
}

void StepperMotor::stepDone() {
    cout << "+stepDone: remains " << m_remaining_steps << " step(s)" << endl;
    --m_remaining_steps;
    m_angle += 10;
}

void StepperMotor::stopMotor() {
    cout << "+stopMotor" << endl;
    m_remaining_steps = 0;
}
