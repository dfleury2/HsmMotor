#pragma once

#include "ui/ui_motorWindow.h"

#include "StepperMotorFsmAction.hpp"

#include <QMainWindow>
#include <QTimer>

// --------------------------------------------------------------------------
class MotorWindow : public QMainWindow, public Ui_MotorWindow {
    Q_OBJECT

public:
    MotorWindow();

    void setup();

signals:

private:
    StepperMotor sm;
    hsm::sm<action::StepperMotorSm, StepperMotor> fsm{sm};

private: /* GUI */
    QTimer* m_timer = nullptr;
};
