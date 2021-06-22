#pragma once

#include <chrono>
#include <thread>

#include <iomanip>
#include <sstream>

#include <QLabel>

#include "Battery.h"
#include "SerialReceiver.h"
#include "SerialPort.h"
#include "CSVLogger.h"
#include "MainWindow.h"
#include "ViHelper.h"

class BatteryMonitor : public SerialListener {

public:
    explicit BatteryMonitor(MainWindow & w, SerialPort & port);
    ~BatteryMonitor();

    void OnReceive(SerialData data) final;
    void checkBattery(Battery & battery, Battery & secondaryBattery, std::chrono::steady_clock::time_point & currentMillis);

    QLabel *m_LabelATemp, *m_LabelACurrent, *m_LabelAVoltage, *m_LabelACharge, *m_LabelAStage, *m_LabelAElapsed;
    QLabel *m_LabelBTemp, *m_LabelBCurrent, *m_LabelBVoltage, *m_LabelBCharge, *m_LabelBStage, *m_LabelBElapsed;

    void Start();
    void Stop();
    void Run();

    void setIdleDuration(int val);

    float getJigTemp() const {
        return m_JigTemperature;
    }

private:
    MainWindow & m_Window;
    SerialPort & m_ArduinoPort;
    Battery m_BatteryA, m_BatteryB;
    CSVLogger m_Logger;
    ViHelper m_Vi;

    int m_IdleDuration;
    std::chrono::time_point<std::chrono::steady_clock> m_LastReceived;
    float m_JigTemperature, m_VRef;

    static const int NUM_SAMPLES_TO_AVERAGE = 10;
    SerialData m_LastMeasurements[NUM_SAMPLES_TO_AVERAGE];
    int m_MeasurementCounter = 0;

    static std::string stringifyDuration(std::chrono::seconds input_seconds);

    void averageSerialData(SerialData * data);

    thread m_Thread;
    volatile bool m_Running = false;
};