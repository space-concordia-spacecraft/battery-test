#pragma once

#include "Battery.h"
#include "SerialReceiver.h"
#include "SerialPort.h"
#include "MainWindow.h"
#include "QLabel.h"
#include <chrono>
#include <thread>

class BatteryMonitor : public SerialListener {
    friend class MainWindow;

public:
    explicit BatteryMonitor(MainWindow & w, SerialPort & port);

    void OnReceive(SerialData data) final;
    void startCharging();
    float m_JigTemperature, m_VRef;
    void checkBattery(Battery battery, Battery secondaryBattery, std::chrono::steady_clock::time_point currentMillis);

    QLabel *m_LabelATemp, *m_LabelACurrent, *m_LabelAVoltage, *m_LabelACharge, *m_LabelAStage;
    QLabel *m_LabelBTemp, *m_LabelBCurrent, *m_LabelBVoltage, *m_LabelBCharge, *m_LabelBStage;

    void Start();
    void Stop();
    void Run();

private:
    Battery m_BatteryA{}, m_BatteryB{};
    MainWindow & m_Window;
    SerialPort m_ArduinoPort;

    thread m_Thread;
    volatile bool m_Running = false;
};