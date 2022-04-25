#pragma once

#define ARDUINO_WAIT_TIME 100
#define MAX_DATA_LENGTH 4096

#include <cstdio>
#include <cstdlib>
#include <mutex>

class SerialPort {

public:
    virtual ~SerialPort() = default;

    virtual int Read(char* buffer, unsigned int buf_size) = 0;
    virtual bool Write(const char* buffer) = 0;

    virtual bool IsConnected() const = 0;
    virtual void Disconnect() = 0;
    virtual void Connect(const char* port) = 0;

    static std::unique_ptr<SerialPort> Create();
};

#if defined(_WIN32)
#include <windows.h>

class SerialPortWindows : public SerialPort {

public:
    ~SerialPortWindows() override;

    int Read(char* buffer, unsigned int buf_size) override;
    bool Write(const char* buffer) override;

    bool IsConnected() const override;
    void Disconnect() override;
    void Connect(const char* port) override;

private:
    HANDLE m_Handle = INVALID_HANDLE_VALUE;
    bool m_Connected = false;
    COMSTAT m_Status{};
    DWORD m_Errors = 0;

};

#elif defined(__unix__)

#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // write(), read(), close()

class SerialPortUnix : public SerialPort {

public:
    ~SerialPortUnix() override;

    int Read(char* buffer, unsigned int buf_size) override;
    bool Write(const char* buffer) override;

    bool IsConnected() override;
    void Disconnect() override;
    void Connect(const char* port) override;

private:
    int m_Handle;
    bool m_Connected;

};

#endif