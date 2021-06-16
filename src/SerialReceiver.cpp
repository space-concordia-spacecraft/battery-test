#include "SerialReceiver.h"


void SerialReceiver::SetArduinoPort(const char* port) {
    m_ArduinoPort = SerialPort(port);
}

void SerialReceiver::SetListener(const SerialListener * listener) {
    m_Listener = listener;
}

void SerialReceiver::Start() {
    m_Thread = thread(&SerialReceiver::Run, this);
    m_Running = true;
}

void SerialReceiver::Stop() {
    m_Thread.join();
    m_Running = false;
}

void SerialReceiver::Run() {
    while (m_Running) {
        if (m_ArduinoPort.isConnected() && m_Listener != nullptr) {
            // Read serial
            char buffer[MAX_DATA_LENGTH];
            memset(buffer, '\0', MAX_DATA_LENGTH);
            m_ArduinoPort.readSerialPort(buffer, MAX_DATA_LENGTH);

            // Split string at commas
            string serialStr(buffer);
            vector<string> tokens(8);
            auto delimiterIndex = string::npos;
            while ((delimiterIndex = serialStr.find(SERIAL_DELIMITER)) != string::npos) {
                tokens.emplace_back(serialStr.substr(0, delimiterIndex));
                serialStr = serialStr.substr(delimiterIndex + 1);
            }

            // Create and pass data struct
            SerialData data{};
            data.m_Data[0] = std::stof(tokens[0]);
            data.m_Data[1] = std::stof(tokens[1]);
            data.m_Data[2] = std::stof(tokens[2]);
            data.m_Data[3] = std::stof(tokens[3]);
            data.m_Data[4] = std::stof(tokens[4]);
            data.m_Data[5] = std::stof(tokens[5]);
            data.m_Data[6] = std::stof(tokens[6]);
            data.m_Data[7] = std::stof(tokens[7]);
            m_Listener->OnReceive(data);
        }
    }
}

