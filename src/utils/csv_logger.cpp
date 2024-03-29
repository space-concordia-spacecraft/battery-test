#include "utils/csv_logger.h"

namespace zeus {
    void CSVLogger::Init() {
        long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        log.open("./log-" + std::to_string(timestamp) + ".csv");
        if (!log.is_open()) {
            std::cout << "ERROR: Could not open log file." << std::endl;
            return;
        }

        log << "Timestamp,";
        log << "Battery A Voltage,";
        log << "Battery A Current,";
        log << "Battery A Temperature,";
        log << "Battery A State,";
        log << "Battery A Progress,";
        log << "Battery A State Duration,";

        log << "Battery B Voltage,";
        log << "Battery B Current,";
        log << "Battery B Temperature,";
        log << "Battery B State,";
        log << "Battery B Progress,";
        log << "Battery B State Duration,";

        log << "Jig Temperature,";
        log << "Capacity" << std::endl;
    }

    void CSVLogger::Close() {
        log.close();
    }

    void CSVLogger::LogState(long long now, const Battery& batteryA, const Battery& batteryB, float jigTemperature, const ViHelper& viHelper) {
        log << now << ",";
        log << batteryA.GetVoltage() << ",";
        log << batteryA.GetCurrent() << ",";
        log << batteryA.GetTemp() << ",";
        log << batteryA.GetSequenceStateLabel() << ",";
        log << batteryA.GetCurrentSequenceState() << ",";
        log << batteryA.GetStateDuration() << ",";

        log << batteryB.GetVoltage() << ",";
        log << batteryB.GetCurrent() << ",";
        log << batteryB.GetTemp() << ",";
        log << batteryB.GetSequenceStateLabel() << ",";
        log << batteryB.GetCurrentSequenceState() << ",";
        log << batteryB.GetStateDuration() << ",";

        log << jigTemperature << ",";
        viHelper.PrintCapacity(log);
    }
}