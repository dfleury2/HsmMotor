#pragma once

#include <iostream>

using namespace std;

class StepperMotor {
public:
    void enableMotor() {
        cout << "+enableMotor" << endl;
        m_isEnable = true;
    }

    void disableMotor() {
        cout << "+disableMotor" << endl;
        m_isEnable = false;
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
        m_angle += 10;
    }

    void stopMotor() {
        cout << "+stopMotor" << endl;
        m_remaining_steps = 0;
    }

    [[nodiscard]] bool isEnable() const { return m_isEnable; }
    [[nodiscard]] int getRemainingSteps() const { return m_remaining_steps; }

    [[nodiscard]] int getAngle() const noexcept { return m_angle % 360; }

private:
    bool m_isEnable{false};
    int m_remaining_steps{};
    int m_angle{};
};
