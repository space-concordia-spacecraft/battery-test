#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor() {

}

void BatteryMonitor::parseData() {
    SerialPort arduino(port);
    if(arduino.isConnected()){
        std::cout << "Connection made" << std::endl << std::endl;
    }

    while (arduino.isConnected()) {
        char buffer[MAX_DATA_LENGTH];
        arduino.readSerialPort(buffer, MAX_DATA_LENGTH);
        std::cout << buffer << std::endl;
    }
}