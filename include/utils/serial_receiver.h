#pragma once

#include <thread>
#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "utils/serial_port.h"

#define SERIAL_VALUE_DELIMITER ','
#define SERIAL_LINE_DELIMITER ';'

using std::thread;
using std::string;
using std::vector;

namespace zeus {
    union SerialData {

        float m_Data[8];

        struct {
            float m_CellA_TM;
            float m_CellA_IM;
            float m_CellA_VM;
            float m_CellB_TM;
            float m_CellB_IM;
            float m_CellB_VM;
            float m_VRef;
            float m_Jig_TM;
        };

    };

    class SerialListener {
    public:
        virtual void OnReceive(SerialData data) = 0;
    };

    class SerialReceiver {

    public:
        explicit SerialReceiver(const char* port = R"(\\.\COM1)");
        explicit SerialReceiver(const SerialPort& mArduinoPort);

        void SetArduinoPort(const char* port);
        void SetListener(SerialListener * listener);

        SerialPort & GetArduinoPort();

        void Start();
        void Stop();
        void Run();

    private:
        SerialPort m_ArduinoPort;
        SerialListener * m_Listener = nullptr;
        thread m_Thread;
        volatile bool m_Running = false;

        string ReadNext();

    };
}