#pragma once

#include <chrono>
#include <thread>

#include "Battery.h"
#include "SerialReceiver.h"
#include "SerialPort.h"
#include "MainWindow.h"
#include <QLabel>

#define COMMAND_CHARGE_A "charge_a"
#define COMMAND_CHARGE_B "charge_b"

#define COMMAND_DISCHARGE_A "discharge_a"
#define COMMAND_DISCHARGE_B "discharge_b"

class BatteryMonitor : public SerialListener {
    friend class MainWindow;

public:
    explicit BatteryMonitor(MainWindow & w, SerialPort & port);

    void OnReceive(SerialData data) final;
    void startCharging();
    float m_JigTemperature, m_VRef;
    void checkBattery(Battery battery, std::chrono::steady_clock::time_point currentMillis);

    QLabel *m_LabelATemp, *m_LabelACurrent, *m_LabelAVoltage, *m_LabelACharge, *m_LabelAStage;
    QLabel *m_LabelBTemp, *m_LabelBCurrent, *m_LabelBVoltage, *m_LabelBCharge, *m_LabelBStage;

    void Start();
    void Stop();
    void Run();

private:
    Battery m_BatteryA{}, m_BatteryB{};
    MainWindow & m_Window;
    SerialPort & m_ArduinoPort;

    thread m_Thread;
    volatile bool m_Running = false;
};