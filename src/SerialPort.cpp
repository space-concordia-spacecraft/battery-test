#include "SerialPort.h"

#include <iostream>

static std::mutex portMutex;

SerialPort::SerialPort(const char* portName) {
    Connect(portName);
}

SerialPort::~SerialPort() {
    Disconnect();
}

int SerialPort::ReadSerialPort(char* buffer, unsigned int buf_size) {
    std::lock_guard guard(portMutex);
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(this->handler, &this->errors, &this->status);

    if (this->status.cbInQue > 0) {
        if (this->status.cbInQue > buf_size) {
            toRead = buf_size;
        } else toRead = this->status.cbInQue;
    }

    if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

    return 0;
}

bool SerialPort::WriteSerialPort(const char* buffer) {
    std::lock_guard guard(portMutex);
    DWORD bytesSent;
    DWORD size = std::string(buffer).size();

    if (!WriteFile(this->handler, (void*) buffer, size, &bytesSent, nullptr)) {
        ClearCommError(this->handler, &this->errors, &this->status);
        return false;
    } else
        return true;
}

bool SerialPort::IsConnected() {
    return this->connected;
}

void SerialPort::Disconnect() {
    std::lock_guard guard(portMutex);
    if (this->connected) {
        this->connected = false;
        CloseHandle(this->handler);
    }
}

void SerialPort::Connect(const char* portName) {
    if (connected)
        Disconnect();

    std::lock_guard guard(portMutex);
    this->connected = false;
    this->handler = CreateFileA(static_cast<LPCSTR>(portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (this->handler == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        } else {
            printf("ERROR!!!");
        }
    } else {
        DCB dcbSerialParameters = { 0 };

        if (!GetCommState(this->handler, &dcbSerialParameters)) {
            printf("failed to get current serial parameters");
        } else {
            dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(handler, &dcbSerialParameters)) {
                printf("ALERT: could not set Serial port parameters\n");
            } else {
                this->connected = true;
                PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
}
