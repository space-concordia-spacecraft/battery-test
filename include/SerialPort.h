#pragma once

#define ARDUINO_WAIT_TIME 100
#define MAX_DATA_LENGTH 4096

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <mutex>

class SerialPort {
private:
    HANDLE handler = INVALID_HANDLE_VALUE;
    bool connected = false;
    COMSTAT status{};
    DWORD errors = 0;

public:
    explicit SerialPort(const char* portName);

    ~SerialPort();

    int ReadSerialPort(char* buffer, unsigned int buf_size);
    bool WriteSerialPort(const char* buffer);

    bool IsConnected();
    void Disconnect();
    void Connect(const char* port);
};