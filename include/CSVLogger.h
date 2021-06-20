#pragma once

#include <fstream>
#include <iostream>

#include "Battery.h"

class BatteryMonitor;

class CSVLogger {

public:
    void Init();
    void LogState(const Battery& batteryA, const Battery& batteryB, const BatteryMonitor& batteryMonitor);
    void Close();

private:
    std::ofstream log;

};