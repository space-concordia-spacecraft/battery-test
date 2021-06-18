#pragma once

#include <thread>
#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "SerialPort.h"

#define SERIAL_VALUE_DELIMITER ','
#define SERIAL_LINE_DELIMITER ';'

using std::thread;
using std::string;
using std::vector;

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

    void SetArduinoPort(const char* port);
    void SetListener(const SerialListener * listener);

    void Start();
    void Stop();
    void Run();

private:
    SerialPort m_ArduinoPort;
    const SerialListener * m_Listener = nullptr;
    thread m_Thread;
    volatile bool m_Running = false;

    string ReadNext();

};