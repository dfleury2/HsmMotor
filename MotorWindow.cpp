#include "MotorWindow.hpp"

#include <QObject>

#include <fmt/format.h>

#include <chrono>

using namespace std;

// --------------------------------------------------------------------------
MotorWindow::MotorWindow() : Ui_MotorWindow() {
    setup();
}

// --------------------------------------------------------------------------
void MotorWindow::setup() {
    setupUi(this);

    QObject::connect(pb_Enable, &QPushButton::pressed, [this] { fsm.process_event(action::enable{}); });
    QObject::connect(pb_Disable, &QPushButton::pressed, [this] { fsm.process_event(action::disable{}); });
    QObject::connect(pb_Stop, &QPushButton::pressed, [this] { /*fsm.process_event(action::stop{}); */ });
    QObject::connect(pb_StepDone, &QPushButton::pressed, [this] { /*fsm.process_event(action::step_done{});*/ });

    QObject::connect(pb_Rotate, &QPushButton::pressed, [this] {
        if (cb_Continuous->isChecked()) {
//            fsm.process_event(action::rotate_continuous{});
        } else {
//            fsm.process_event(action::rotate_step{.steps = sp_Steps->value()});
        }
    });

    m_timer = new QTimer(this);
    QObject::connect(m_timer, &QTimer::timeout, [this] {
//        fsm.process_event(action::step_done{});

        cb_Tick->setChecked(!cb_Tick->isChecked());
        cb_IsEnable->setChecked(sm.isEnable());
        dial->setValue(sm.getAngle());
        l_Steps_Remais->setText(fmt::format("Step(s) remains: {}", sm.getRemainingSteps()).c_str());
    });

    m_timer->start(std::chrono::milliseconds(250));
}
