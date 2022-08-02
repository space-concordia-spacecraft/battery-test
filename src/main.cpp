#include <core/application.h>
#include <windows/general_window.h>
#include "utils/serial_receiver.h"
#include "utils/battery_monitor.h"
#include "windows/battery_window.h"

int main(int argc, char** argv) {
    zeus::SerialReceiver receiver;

    zeus::BatteryMonitor windowBattery(receiver.GetArduinoPort());

    receiver.SetListener(&windowBattery);
    receiver.Start();

    auto* zeus = new zeus::Application();
    zeus->AddWindow(new zeus::GeneralWindow("General", windowBattery, receiver));
    zeus->AddWindow(new zeus::BatteryWindow("Battery A", windowBattery.m_BatteryA));
    zeus->AddWindow(new zeus::BatteryWindow("Battery B", windowBattery.m_BatteryB));
    zeus->Run();
    delete zeus;

    windowBattery.Stop();
    receiver.Stop();

    return 0;
}