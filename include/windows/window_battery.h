#pragma once

#include "core/ui_window.h"
#include "battery/battery.h"
#include "utils/serial_receiver.h"
#include "utils/serial_port.h"
#include "utils/csv_logger.h"
#include "utils/vi_helper.h"

namespace zeus {

    class WindowBattery : public zeus::UIWindow {

    public:
        //!
        //! \param name
        explicit WindowBattery(string name, SerialPort & port);

        //!
        ~WindowBattery();

        //!
        //! \param data
        void OnReceive(SerialData data);

        //!
        void RenderGUI() override;

        //!
        //! \return ZS_SUCCESS if the process was successful. Otherwise returns ZS_ERROR.
        ZsResult Start();

        //!
        //! \return ZS_SUCCESS if the process was successful. Otherwise returns ZS_ERROR.
        ZsResult Stop();

        //!
        void Run();

        //!
        //! \param battery
        //! \param secondaryBattery
        void CheckBattery(Battery& battery, Battery& secondaryBattery) const;

        //!
        //! \return
        string GetName() const;

    private:
        SerialPort & m_ArduinoPort;
        Battery m_BatteryA, m_BatteryB;
        CSVLogger m_Logger;
        ViHelper m_Vi;

        int m_IdleDuration = 600;
        void SetIdleDuration(int val);
        std::chrono::time_point<std::chrono::steady_clock> m_LastReceived;
        float m_JigTemperature, m_VRef;

        static const int NUM_SAMPLES_TO_AVERAGE = 10;
        static const int PRECISION = 3;
        SerialData m_LastMeasurements[NUM_SAMPLES_TO_AVERAGE];
        int m_MeasurementCounter = 0;

        static std::string StringifyDuration(std::chrono::seconds input_seconds);
        void AverageSerialData(SerialData * data);

        thread m_Thread;
        volatile bool m_Running = false;

        const string m_WindowName;
    };
}