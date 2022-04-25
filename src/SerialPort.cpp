#include "SerialPort.h"

static std::mutex portMutex;

std::unique_ptr<SerialPort> SerialPort::Create() {
#if defined(_WIN32)
    return std::make_unique<SerialPortWindows>();
#elif defined(__unix__)
    return std::make_unique<SerialPortUnix>();
#endif
}

#if defined(_WIN32)

SerialPortWindows::~SerialPortWindows() {
    SerialPortWindows::Disconnect();
}

int SerialPortWindows::Read(char* buffer, unsigned int buf_size) {
    std::lock_guard guard(portMutex);
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(m_Handle, &m_Errors, &m_Status);

    if (m_Status.cbInQue > 0) {
        if (m_Status.cbInQue > buf_size) {
            toRead = buf_size;
        } else {
            toRead = m_Status.cbInQue;
        }
    }

    if (ReadFile(m_Handle, buffer, toRead, &bytesRead, nullptr))
        return (int)bytesRead;

    return 0;
}

bool SerialPortWindows::Write(const char* buffer) {
    std::lock_guard guard(portMutex);
    DWORD bytesSent;
    DWORD size = std::string(buffer).size();

    if (!WriteFile(m_Handle, buffer, size, &bytesSent, nullptr)) {
        ClearCommError(m_Handle, &m_Errors, &m_Status);
        return false;
    } else {
        return true;
    }
}

bool SerialPortWindows::IsConnected() const {
    return m_Connected;
}

void SerialPortWindows::Disconnect() {
    std::lock_guard guard(portMutex);
    if (m_Connected) {
        m_Connected = false;
        CloseHandle(m_Handle);
    }
}

void SerialPortWindows::Connect(const char* portName) {
    if (m_Connected)
        Disconnect();

    std::lock_guard guard(portMutex);
    m_Connected = false;
    m_Handle = CreateFileA(static_cast<LPCSTR>(portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                nullptr,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                 nullptr);
    if (m_Handle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        } else {
            printf("ERROR!!!");
        }
    } else {
        DCB dcbSerialParameters = { 0 };

        if (!GetCommState(m_Handle, &dcbSerialParameters)) {
            printf("failed to get current serial parameters");
        } else {
            dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if (!SetCommState(m_Handle, &dcbSerialParameters)) {
                printf("ALERT: could not set Serial port parameters\n");
            } else {
                m_Connected = true;
                PurgeComm(m_Handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
}

#elif defined(__unix__)

SerialPortUnix::~SerialPortUnix() {
    SerialPortUnix::Disconnect();
}

int SerialPortUnix::Read(char* buffer, unsigned int size) {
    std::lock_guard guard(portMutex);
    return read(m_Handle, buffer, size);
}

bool SerialPortUnix::Write(const char* buffer) {
    std::lock_guard guard(portMutex);
    size_t bufferSize = std::string(buffer).size();
    size_t nBytesWritten = write(m_Handle, buffer, bufferSize);
    return nBytesWritten == bufferSize;
}

bool SerialPortUnix::IsConnected() const {
    return m_Connected;
}

void SerialPortUnix::Disconnect() {
    std::lock_guard guard(portMutex);
    if (m_Connected) {
        close(m_Handle);
        m_Connected = false;
    }
}

void SerialPortUnix::Connect(const char* port) {
    if (m_Connected)
        Disconnect();

    std::lock_guard guard(portMutex);
    m_Handle = open(port, O_RDWR | O_NOCTTY);
    if (m_Handle == -1) {
        printf("ERROR: Couldn't open serial connection to port %s.", port);
        return;
    }
    m_Connected = true;
}

#endif