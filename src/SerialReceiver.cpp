#include "SerialReceiver.h"

SerialReceiver::SerialReceiver(const char* port)
        : m_ArduinoPort(port) {}

void SerialReceiver::SetArduinoPort(const char* port) {
    m_ArduinoPort.disconnect();
    m_ArduinoPort = SerialPort(port);
}

void SerialReceiver::SetListener(SerialListener* listener) {
    m_Listener = listener;
}

void SerialReceiver::Start() {
    m_Running = true;
    m_Thread = thread(&SerialReceiver::Run, this);
}

void SerialReceiver::Stop() {
    m_Running = false;
    m_Thread.join();
}

void SerialReceiver::Run() {

    while (m_Running) {

        std::cout << m_ArduinoPort.isConnected() << std::endl;

        if (m_ArduinoPort.isConnected() && m_Listener != nullptr) {
            // Read next serial line and split at commas
            string serialStr = ReadNext();
            serialStr = serialStr.substr(0, serialStr.find('\n'));
            vector<string> tokens;
            auto delimiterIndex = string::npos;
            while ((delimiterIndex = serialStr.find(SERIAL_VALUE_DELIMITER)) != string::npos) {
                tokens.emplace_back(serialStr.substr(0, delimiterIndex));
                serialStr = serialStr.substr(delimiterIndex + 1);
            }
            tokens.emplace_back(serialStr);

            if (tokens.size() != 8)
                continue;

            // Create and pass data struct
            SerialData data{};
            for (int i = 0; i < 8; i++) {
                try {
                    data.m_Data[i] = std::stof(tokens[i]);
                } catch (const std::exception& ex) {
                    data.m_Data[i] = 0;
                }
            }

            m_Listener->OnReceive(data);
        }
    }
    std::cout << "dead" << std::endl;
}

string SerialReceiver::ReadNext() {
    static string next;

    char tempBuffer[MAX_DATA_LENGTH];

    std::stringstream ss;

    while (true) {
        int nBytes;
        string str;
        if (!next.empty()) {
            str = next;
            next = "";
        } else {
            nBytes = m_ArduinoPort.readSerialPort(tempBuffer, MAX_DATA_LENGTH);
            str = string(tempBuffer, nBytes);
        }
        str = std::regex_replace(str, std::regex("\\s+"), "");
        int delimiterIndex = str.find(SERIAL_LINE_DELIMITER);
        if (delimiterIndex != string::npos) {
            ss << str.substr(0, delimiterIndex);
            next = str.substr(delimiterIndex + 1);
            return ss.str();
        }
        ss << str;
    }

}