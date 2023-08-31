#pragma once

class StepperMotor {
public:
    void enableMotor();
    void disableMotor();
    void rotateStep(int step);
    void rotateContinuous();
    void stepDone();
    void stopMotor();

    [[nodiscard]] bool isEnable() const { return m_isEnable; }
    [[nodiscard]] int getRemainingSteps() const { return m_remaining_steps; }

    [[nodiscard]] int getAngle() const noexcept { return m_angle % 360; }

private:
    bool m_isEnable{false};
    int m_remaining_steps{};
    int m_angle{};
};
