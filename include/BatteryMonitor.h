#pragma once
#include "Battery.h"
#include "SerialPort.h"

class BatteryMonitor {

    Battery batteryA{}, batteryB{};

    BatteryMonitor();
    void parseData();
};