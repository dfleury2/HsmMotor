#pragma once

#include <iostream>

using namespace std;

class StepperMotor {
public:
    void enableMotor() {
        cout << "+enableMotor" << endl;
    }

    void disableMotor() {
        cout << "+disableMotor" << endl;
    }

    void rotateStep(uint32_t step) {
        cout << "+rotateStep: " << step << endl;
        m_remaining_steps = step;
    }

    void rotateContinuous() {
        cout << "+rotateContinuous" << endl;
    }

    void stepDone() {
        cout << "+stepDone: remains " << m_remaining_steps << " step(s)" << endl;
        --m_remaining_steps;
    }

    void stopMotor() {
        cout << "+stopMotor" << endl;
        m_remaining_steps = 0;
    }

    [[nodiscard]] int getRemainingSteps() const { return m_remaining_steps; }

private:
    int m_remaining_steps{};
};
