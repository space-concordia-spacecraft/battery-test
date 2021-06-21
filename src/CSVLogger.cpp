#include "CSVLogger.h"
#include "BatteryMonitor.h"

void CSVLogger::Init() {
    long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    log.open("./log-" + std::to_string(timestamp) + ".csv");
    if (!log.is_open()) {
        std::cout << "ERROR: Could not open log file." << std::endl;
        return;
    }

    log << "Battery A Voltage,";
    log << "Battery A Current,";
    log << "Battery A Temperature,";

    log << "Battery B Voltage,";
    log << "Battery B Current,";
    log << "Battery B Temperature,";

    log << "Jig Temperature" << std::endl;
}

void CSVLogger::Close() {
    log.close();
}

void CSVLogger::LogState(const Battery& batteryA, const Battery& batteryB, const BatteryMonitor& batteryMonitor) {
    log << batteryA.getVolt() << ",";
    log << batteryA.getCurrent() << ",";
    log << batteryA.getTemp() << ",";

    log << batteryB.getVolt() << ",";
    log << batteryB.getCurrent() << ",";
    log << batteryB.getTemp() << ",";

    log << batteryMonitor.getJigTemp() << std::endl;
}