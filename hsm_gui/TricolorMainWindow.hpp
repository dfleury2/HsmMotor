#pragma once

#include "ui/ui_TricolorMainWindow.h"
#include "TricolorLight.hpp"

#include <fmt/chrono.h>

#include <QTimer>
#include <QMainWindow>

class TricolorMainWindow : public QMainWindow, protected Ui::Form
{
  public:
    explicit TricolorMainWindow() : Ui::Form()
    {
        setupUi(this);

        m_timer.setInterval(std::chrono::milliseconds(25));
        QObject::connect(&m_timer, &QTimer::timeout, this, &TricolorMainWindow::onIdle);
        m_timer.start();

        QObject::connect(bStartStop, &QPushButton::pressed, this, &TricolorMainWindow::onStartStop);

        label->clear();
    }

    void onStartStop()
    {
        m_stateMachine.process_event(StartStop{});

        m_Started = !m_Started;
        if (m_Started) {
            m_startTime = std::chrono::system_clock::now();
        }

        displayLights();
    }

    void onIdle()
    {
        m_stateMachine.process_event(Tick{});

        displayLights();
    }

  private:
    void displayLights()
    {
        cbRed->setCheckState(m_triColorLight.red ? Qt::Checked : Qt::Unchecked);
        cbOrange->setCheckState(m_triColorLight.orange ? Qt::Checked : Qt::Unchecked);
        cbGreen->setCheckState(m_triColorLight.green ? Qt::Checked : Qt::Unchecked);

        if (m_Started) {
            double elapsed = duration_cast<duration<double>>(std::chrono::system_clock::now() - m_startTime).count();
            label->setText(fmt::format("{:>15.6f}", elapsed).c_str());
        }
    }

  private:
    TriColorLight m_triColorLight;
    hsm::sm<TriColorLightSm, TriColorLight> m_stateMachine{m_triColorLight};

    QTimer m_timer;
    bool m_Started = false;
    std::chrono::system_clock::time_point m_startTime{};
};
