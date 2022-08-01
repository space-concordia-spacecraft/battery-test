#include <core/application.h>
#include "utils/serial_receiver.h"
#include "windows/window_battery.h"

int main(int argc, char** argv) {

    zeus::SerialReceiver receiver;

    zeus::WindowBattery windowBattery("Window Battery", receiver.GetArduinoPort());

    receiver.SetListener(&windowBattery);
    receiver.Start();

    auto* zeus = new zeus::Application();
    zeus->Run();
    delete zeus;

    windowBattery.Stop();
    receiver.Stop();

    return 0;
}