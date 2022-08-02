#pragma once

#include "utils/battery_monitor.h"

namespace zeus {
    BatteryMonitor::BatteryMonitor(SerialPort& port)
            : m_ArduinoPort(port), m_BatteryA(port), m_BatteryB(port) {
        // Sets the arduino port for each battery so that they can communicate with the hardware
        m_BatteryA.SetArduinoPort(m_ArduinoPort);
        m_BatteryB.SetArduinoPort(m_ArduinoPort);

        // Sets the letters for each battery so that they can be identified.
        m_BatteryA.SetLetter("a");
        m_BatteryB.SetLetter("b");

        // Initializes the electronic load helper
        m_Vi.Init();
    }

    BatteryMonitor::~BatteryMonitor() {
        // Closes any services that were open.
        m_Logger.Close();
        m_Vi.Close();
    }

    void BatteryMonitor::OnReceive(SerialData data) {
        // Store results in m_BatteryA and m_BatteryB
        m_LastReceived = std::chrono::high_resolution_clock::now();
        m_LastMeasurements[m_MeasurementCounter] = data;

        if (m_MeasurementCounter == NUM_SAMPLES_TO_AVERAGE - 1) {
            SerialData averageData{};
            AverageSerialData(&averageData);

            if (m_BatteryA.GetState() == Battery::IDLE) {
                m_BatteryA.SetIdleCurrent(averageData.m_CellA_IM);
            }

            if (m_BatteryB.GetState() == Battery::IDLE) {
                m_BatteryB.SetIdleCurrent(averageData.m_CellB_IM);
            }

            m_BatteryA.SetTemp(averageData.m_CellA_TM);
            m_BatteryA.SetVoltage(averageData.m_CellA_VM);
            m_BatteryA.SetCurrent(averageData.m_CellA_IM);

            m_BatteryB.SetTemp(averageData.m_CellB_TM);
            m_BatteryB.SetVoltage(averageData.m_CellB_VM);
            m_BatteryB.SetCurrent(averageData.m_CellB_IM);

            m_JigTemperature = averageData.m_Jig_TM;
            m_VRef = averageData.m_VRef;
        }
        m_MeasurementCounter = (m_MeasurementCounter + 1) % NUM_SAMPLES_TO_AVERAGE;
    }

    ZsResult BatteryMonitor::Start() {
        if (m_Running)
            return ZS_ERROR;

        // Battery A should start with charging. By default they start in idle.
        m_BatteryA.SetCurrentSequenceStep(1);

        // Restarts Watchdog Timer, otherwise it will never listen to the following commands
        m_ArduinoPort.WriteSerialPort("ping\n");

        // Begins testing.
        m_BatteryA.StartStateTest();
        m_BatteryB.StartStateTest();

        // Initializes logger
        m_Logger.Init();

        m_Running = true;
        m_Thread = thread(&BatteryMonitor::Run, this);

        std::cout << "Starting Battery Monitor" << std::endl;

        return ZS_SUCCESS;
    }

    ZsResult BatteryMonitor::Stop() {
        if (!m_Running)
            return ZS_ERROR;

        // Stops electronic load and logger.
        m_Vi.StopLoad();
        m_Logger.Close();

        // Sets both batteries to idle.
        m_BatteryA.SetCurrentSequenceStep(0);
        m_BatteryB.SetCurrentSequenceStep(0);

        m_BatteryA.Idle();
        m_BatteryB.Idle();

        m_Running = false;
        m_Thread.join();

        std::cout << "Stopping Battery Monitor" << std::endl;

        return ZS_SUCCESS;
    }

    void BatteryMonitor::Run() {
        // start timer
        auto startTime = std::chrono::high_resolution_clock::now();

        // Start the timer for the batteries
        m_BatteryA.ResetStateStartTime();
        m_BatteryB.ResetStateStartTime();

        auto lastLog = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();

        m_LastReceived = std::chrono::high_resolution_clock::now();

        std::cout << "Running Battery Monitor" << std::endl;

        // pings to the arduino
        m_ArduinoPort.WriteSerialPort("ping\n");

        while (m_Running && (!m_BatteryA.IsCompleted() || !m_BatteryB.IsCompleted())) {

            now = std::chrono::high_resolution_clock::now();

            m_ArduinoPort.WriteSerialPort("ping\n");

            if (std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::high_resolution_clock::now() - m_LastReceived).count() >= 5) {
                m_BatteryA.Idle();
                m_BatteryB.Idle();

                m_Logger.Close();

                std::cout << "ERROR: Haven't Received Message in a while" << std::endl;

                Stop();
                return;
            }

            CheckBattery(m_BatteryA, m_BatteryB);
            CheckBattery(m_BatteryB, m_BatteryA);

            now = std::chrono::high_resolution_clock::now();

            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLog).count() >= 10) {
                m_Logger.LogState(m_BatteryA, m_BatteryB, 0.0, m_Vi);
                lastLog = std::chrono::high_resolution_clock::now();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        m_Running = false;
        m_Logger.Close();
    }

    void BatteryMonitor::CheckBattery(Battery& battery,
                                      Battery& secondaryBattery) const {
        if (battery.GetState() == Battery::CHARGING &&
            (std::abs(battery.GetCurrent()) <= Battery::LOWEST_CURRENT ||
             battery.GetVoltage() >= Battery::HIGHEST_VOLTAGE) &&
            battery.GetStateDuration().count() >= 5) {

            battery.CompleteState();
            battery.ResetStateStartTime();

            std::cout << "UPDATE: Battery " << battery.GetLetter() << " is exiting charge and is now " << battery.GetSequenceStateLabel() << "." << std::endl;

        } else if ( battery.GetState() == Battery::DISCHARGING &&
                    ( battery.GetVoltage() <= Battery::LOWEST_VOLTAGE || battery.GetCurrent() < 0 ) &&
                    battery.GetStateDuration().count() >= 5) {

            m_Vi.StopLoad();
            battery.CompleteState();

            battery.ResetStateStartTime();

            std::cout << "UPDATE: Battery " << battery.GetLetter() << " is exiting discharge and is now " << battery.GetSequenceStateLabel() << "." << std::endl;

        } else if (battery.GetState() == Battery::IDLE || battery.IsCompleted()) {
            if (battery.GetStateDuration().count() >= m_IdleDuration || battery.IsCompleted()) {
                battery.SetReady(true);
            }

            if (battery.IsReady() && secondaryBattery.IsReady()) {
                battery.CompleteState();
                secondaryBattery.CompleteState();

                // restart timers
                battery.ResetStateStartTime();
                secondaryBattery.ResetStateStartTime();

                battery.SetReady(false);
                secondaryBattery.SetReady(false);

                std::cout << "UPDATE: Battery " << battery.GetLetter() << " is now " << battery.GetSequenceStateLabel() << "." << std::endl;

                if (battery.GetState() == Battery::DISCHARGING ||
                    secondaryBattery.GetState() == Battery::DISCHARGING) {
                    m_Vi.StartLoad();
                    std::cout << "UPDATE: Starting Load." << std::endl;
                }

            }

        }
    }

    std::string BatteryMonitor::StringifyDuration(std::chrono::seconds input_seconds) {
        using namespace std::chrono;
        auto h = duration_cast<hours>(input_seconds);
        input_seconds -= h;
        auto m = duration_cast<minutes>(input_seconds);
        input_seconds -= m;
        auto s = duration_cast<seconds>(input_seconds);

        auto hc = h.count();
        auto mc = m.count();
        auto sc = s.count();

        std::stringstream ss;
        ss.fill('0');

        ss << std::setw(2) << hc << ":" << std::setw(2) << mc << ":" << std::setw(2) << sc;

        return ss.str();
    }

    void BatteryMonitor::AverageSerialData(SerialData* data) {
        *data = { 0, 0, 0, 0, 0, 0, 0, 0 };
        for (auto& m_LastMeasurement : m_LastMeasurements) {
            for (int j = 0; j < 8; j++) {
                data->m_Data[j] += m_LastMeasurement.m_Data[j];
            }
        }
        for (float& d : data->m_Data) {
            d /= static_cast<float>(NUM_SAMPLES_TO_AVERAGE);
        }
    }

    void BatteryMonitor::SetIdleDuration(int val) {
        this->m_IdleDuration = val;
    }
}
