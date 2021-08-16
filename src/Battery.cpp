#include "Battery.h"

Battery::Battery(SerialPort& port): m_ArduinoPort(port) {}

int Battery::GetState() {
    return m_Sequence[m_CurrentSequenceStep];
}

std::string Battery::GetSequenceStateLabel() {
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

int Battery::GetCurrentSequenceState() {
    return this->m_CurrentSequenceStep;
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
    m_ArduinoPort.writeSerialPort((std::string(COMMAND_CHARGE) + m_Letter + "\n").c_str());
}

void Battery::Discharge() {
    m_ArduinoPort.writeSerialPort((std::string(COMMAND_DISCHARGE) + m_Letter + "\n").c_str());
}

void Battery::Idle() {
    m_ArduinoPort.writeSerialPort((std::string(COMMAND_IDLE) + m_Letter + "\n").c_str());
}

void Battery::SetTemp(float volt) {
    this->m_Temperature = interpolate(m_TemperatureVolts, m_TemperatureValue, volt);
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