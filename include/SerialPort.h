#pragma once

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 4096

#include <windows.h>
#include <cstdio>
#include <cstdlib>

class SerialPort
{
private:
    HANDLE handler;
    bool connected;
    COMSTAT status;
    DWORD errors;
public:
    explicit SerialPort(const char *portName);
    ~SerialPort();

    int readSerialPort(char *buffer, unsigned int buf_size);
    bool writeSerialPort(char *buffer, unsigned int buf_size);
    bool isConnected();
    void disconnect();
};