#include "Battery.h"

Battery::Battery(SerialPort& port): m_ArduinoPort(port) {}

int Battery::GetState() const {
    return m_Sequence[m_CurrentSequenceStep];
}

int Battery::GetCurrentSequenceState() const {
    return this->m_CurrentSequenceStep;
}

std::chrono::seconds Battery::GetStateDuration() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - m_StateStartTime);
}

std::string Battery::GetSequenceStateLabel() const {
    if(m_CurrentSequenceStep == 10)
        return "Complete";

    if(this->GetState() == CHARGING) {
        return "Charging";
    } else if(this->GetState() == DISCHARGING) {
        return "Discharging";
    } else if(this->GetState() == IDLE) {
        return "Idle";
    }
}

void Battery::CompleteState() {
    m_CurrentSequenceStep ++;

    if(m_CurrentSequenceStep == 10) {
        std::cout << "UPDATE: Test Completed" << std::endl;
    }

    StartStateTest();
}

bool Battery::IsCompleted() {
    return m_CurrentSequenceStep == 10;
}

bool Battery::IsReady() {
    return this->m_Ready;
}

void Battery::StartStateTest() {
    switch(GetState()) {
        case CHARGING:
            Charge();
            break;
        case DISCHARGING:
            Discharge();
            break;
        case IDLE:
            Idle();
            break;
    }
}

void Battery::Charge() {
    bool result = m_ArduinoPort.WriteSerialPort((std::string(COMMAND_CHARGE) + m_Letter + "\n").c_str());
    std::cout << "Sending " << std::string(COMMAND_CHARGE) + m_Letter << " to the arduino" << std::endl;
    if(result)
        std::cout << "Succesfully started charging the battery" << std::endl;
    else
        std::cout << "ERROR: Could not start charging the battery" << std::endl;
}

void Battery::Discharge() {
    bool result = m_ArduinoPort.WriteSerialPort((std::string(COMMAND_DISCHARGE) + m_Letter + "\n").c_str());
    std::cout << "Sending " << std::string(COMMAND_DISCHARGE) + m_Letter << " to the arduino" << std::endl;
    if(result)
        std::cout << "Succesfully started discharging the battery" << std::endl;
    else
        std::cout << "ERROR: Could not start discharging the battery" << std::endl;
}

void Battery::Idle() {
    bool result = m_ArduinoPort.WriteSerialPort((std::string(COMMAND_IDLE) + m_Letter + "\n").c_str());
    std::cout << "Sending " << std::string(COMMAND_IDLE) + m_Letter << " to the arduino" << std::endl;
    if(result)
        std::cout << "Succesfully idled the battery" << std::endl;
    else
        std::cout << "ERROR: Could not start idle the battery" << std::endl;
}

void Battery::SetTemp(float volt) {
    this->m_Temperature = Interpolate(m_TemperatureVolts, m_TemperatureValue, volt);
}

void Battery::SetVoltage(float volt) {
    this->m_Voltage = volt;
}

void Battery::SetCurrent(float current) {
    this->m_Current = ( current - this->m_IdleCurrent ) / 0.681f;
}

void Battery::SetIdleCurrent(float current) {
    this->m_IdleCurrent = current;
}

void Battery::SetLetter(const std::string& letter) {
    this->m_Letter = letter;
}

void Battery::SetArduinoPort(SerialPort& port) {
    this->m_ArduinoPort = port;
}

void Battery::SetReady(bool ready) {
    this->m_Ready = ready;
}

void Battery::SetCurrentSequenceStep(int index) {
    this->m_CurrentSequenceStep = index;
}

void Battery::ResetStateStartTime() {
    m_StateStartTime = std::chrono::high_resolution_clock::now();
}

std::chrono::steady_clock::time_point Battery::GetStateStartTime() {
    return m_StateStartTime;
}

float Battery::GetTemp() const {
    return this->m_Temperature;
}

float Battery::GetVoltage() const {
    return this->m_Voltage;
}

float Battery::GetCurrent() const {
    return this->m_Current;
}

float Battery::GetIdleCurrent() const {
    return this->m_IdleCurrent;
}

std::string Battery::GetLetter() const {
    return this->m_Letter;
}