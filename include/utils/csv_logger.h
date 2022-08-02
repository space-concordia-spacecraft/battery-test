#pragma once

#include <fstream>
#include <iostream>

#include "battery/battery.h"
#include "utils/vi_helper.h"

class BatteryMonitor;

namespace zeus {
    class CSVLogger {

    public:
        void Init();
        void LogState(const Battery& batteryA, const Battery& batteryB, float jigTemperature, const ViHelper& viHelper);
        void Close();

    private:
        std::ofstream log;
    };
}
