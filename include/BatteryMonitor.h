#pragma once

#include "Battery.h"
#include "SerialReceiver.h"
#include "SerialPort.h"
#include "MainWindow.h"
#include "QLabel.h"
#include <thread>

class BatteryMonitor : SerialListener {
    friend class MainWindow;

public:
    BatteryMonitor(MainWindow & w, SerialPort & port);

    virtual void OnReceive(SerialData data) override final;
    void startCharging();
    float m_JigTemperature, m_VRef;

    QLabel *m_LabelATemp, *m_LabelACurrent, *m_LabelAVoltage, *m_LabelACharge, *m_LabelAStage;
    QLabel *m_LabelBTemp, *m_LabelBCurrent, *m_LabelBVoltage, *m_LabelBCharge, *m_LabelBStage;

    void Start();
    void Stop();
    void Run();

private:
    Battery m_BatteryA{}, m_BatteryB{};
    MainWindow & m_Window;

    thread m_Thread;
    volatile bool m_Running = false;
};