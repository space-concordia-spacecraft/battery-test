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
    /**
     * Constructor that initializes all labels, sets up the electronic load helper, and sets up the battery objects.
     * @param w - Window object used for the labels.
     * @param port - SerialPort object to communicate with the battery test jig hardware.
     */
    explicit BatteryMonitor(MainWindow & w, SerialPort & port);

    /**
     * Destructor for the BatteryMonitor class. Destroys logger and electronic load helper.
     */
    ~BatteryMonitor();

    void OnReceive(SerialData data) final;
    void UpdateBatteryData(Battery& battery, float temp, float current, float voltage);
    void CheckBattery(Battery& battery,
                      Battery& secondaryBattery);

    QLabel *m_LabelTimeElapsed;

    QLabel *m_LabelATemp, *m_LabelACurrent, *m_LabelAVoltage, *m_LabelAStage, *m_LabelAElapsed, *m_LabelAProgress;
    QLabel *m_LabelBTemp, *m_LabelBCurrent, *m_LabelBVoltage, *m_LabelBStage, *m_LabelBElapsed, *m_LabelBProgress;

    void Start();
    void Stop();
    void Run();

    void SetIdleDuration(int val);

    float GetJigTemp() const {
        return m_JigTemperature;
    }

private:
    MainWindow & m_Window;
    SerialPort & m_ArduinoPort;
    Battery m_BatteryA, m_BatteryB;
    CSVLogger m_Logger;
    ViHelper m_Vi;

    int m_IdleDuration = 600;
    std::chrono::time_point<std::chrono::steady_clock> m_LastReceived;
    float m_JigTemperature, m_VRef;

    static const int NUM_SAMPLES_TO_AVERAGE = 10;
    SerialData m_LastMeasurements[NUM_SAMPLES_TO_AVERAGE];
    int m_MeasurementCounter = 0;

    static std::string StringifyDuration(std::chrono::seconds input_seconds);

    void AverageSerialData(SerialData * data);

    thread m_Thread;
    volatile bool m_Running = false;
};