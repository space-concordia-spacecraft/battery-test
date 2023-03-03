#pragma once

#include "core/ui_window.h"
#include "utils/battery_monitor.h"
#include "utils/serial_receiver.h"

namespace zeus {
    static const char * COM_PORTS[] = {
            R"(\\.\COM1)",
            R"(\\.\COM2)",
            R"(\\.\COM3)",
            R"(\\.\COM4)",
            R"(\\.\COM5)",
            R"(\\.\COM6)",
            R"(\\.\COM7)",
            R"(\\.\COM8)",
            R"(\\.\COM9)",
            R"(\\.\COM10)",
            R"(\\.\COM11)",
            R"(\\.\COM12)",
            R"(\\.\COM13)",
            R"(\\.\COM14)",
            R"(\\.\COM15)",
            R"(\\.\COM16)",
    };

    class GeneralWindow : public UIWindow {
    public:
        GeneralWindow(std::string name, BatteryMonitor& batteryMonitor, SerialReceiver& receiver);

        void RenderGUI() override;

        std::string GetName() const override;

    private:
        const std::string m_WindowName;
        BatteryMonitor& m_BatteryMonitor;
        SerialReceiver& m_SerialReceiver;
        int m_SelectedCOMPortIndex{};
    };

}
