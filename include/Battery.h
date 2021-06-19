#pragma once

#include "Utils.h"
#include "SerialPort.h"

#define COMMAND_CHARGE "charge_"

#define COMMAND_DISCHARGE "discharge_"

#define COMMAND_IDLE "idle_"

struct Battery {

public:
    // Charge, Discharge, Charge, Discharge, Charge
    const static int IDLE0 = 0;
    const static int CHARGE1 = 1;
    const static int IDLE1 = 2;
    const static int DISCHARGE1 = 3;
    const static int IDLE2 = 4;
    const static int CHARGE2 = 5;
    const static int IDLE3 = 6;
    const static int DISCHARGE2 = 7;
    const static int IDLE4 = 8;
    const static int CHARGE3 = 9;
    const static int DONE = 10;

    const static int CHARGING = 0;
    const static int DISCHARGING = 1;
    const static int IDLE = 2;

    constexpr const static float LOWEST_VOLTAGE = 3.0f;
    constexpr const static float HIGHEST_VOLTAGE = 4.2f;

    explicit Battery(SerialPort& port) : m_ArduinoPort(port) {}

    int getGeneralState() {
        if(this->m_CurrentState == IDLE0 || this->m_CurrentState == IDLE1 || this->m_CurrentState == IDLE2 || this->m_CurrentState == IDLE3 || this->m_CurrentState == IDLE4)
            return IDLE;
        else if(this->m_CurrentState == CHARGE1 || this->m_CurrentState == CHARGE2 || this->m_CurrentState == CHARGE3)
            return CHARGING;
        else
            return DISCHARGING;
    };

    bool isIdle() {
        return this->m_CurrentState == IDLE0 || this->m_CurrentState == IDLE1 || this->m_CurrentState == IDLE2 || this->m_CurrentState == IDLE3 || this->m_CurrentState == IDLE4;
    }

    void charge() {
        m_ArduinoPort.writeSerialPort((std::string(COMMAND_CHARGE) + m_Letter).c_str());
    }

    void discharge() {
        m_ArduinoPort.writeSerialPort((std::string(COMMAND_DISCHARGE) + m_Letter).c_str());
    }

    void idle() {
        m_ArduinoPort.writeSerialPort((std::string(COMMAND_IDLE) + m_Letter).c_str());
    }

    void goNext() {
        m_CurrentState = ( m_CurrentState + 1 ) % 9;

        int state = this->getGeneralState();
        if(state == CHARGING) {
            charge();
        } else if(state == DISCHARGING) {
            discharge();
        } else if (state == IDLE) {
            idle();
        }
    }

    void setTemp(float data) {
        this->m_Temperature = interpolate(m_TemperatureVolts, m_TemperatureValue, data);
    }

    void setVoltage(float data) {
        this->m_Voltage = data;
        this->m_Charge = 100.0 * ( data - LOWEST_VOLTAGE )/( HIGHEST_VOLTAGE - LOWEST_VOLTAGE );
    }

    void setCurrent(float data) {
        this->m_Current = ( data - this->m_IdleCurrent ) / 0.681;
    }

    void setIdleCurrent(float data) {
        this->m_IdleCurrent = data;
    }

    void setState(int state) {
        this->m_CurrentState = state;
    }

    void setCompleted(bool completed) {
        this->m_Completed = completed;
    }

    void setReadyForNext(bool ready) {
        this->m_ReadyForNext = ready;
    }

    void setArduinoPort(SerialPort port) {
        this->m_ArduinoPort = port;
    }

    void setLetter(std::string letter) {
        this->m_Letter = letter;
    }

    float getTemp() {
        return this->m_Temperature;
    }

    float getVolt() {
        return this->m_Voltage;
    }

    float getCurrent() {
        return this->m_Current;
    }

    float getIdleCurrent() {
        return this->m_IdleCurrent;
    }

    int getState() {
        return this->m_CurrentState;
    }

    bool isCompleted() {
        return this->m_Completed;
    }

    bool isReadyForNext() {
        return this->m_ReadyForNext;
    }

private:
    int m_CurrentState = CHARGE1;

    std::string m_Letter;
    SerialPort & m_ArduinoPort;

    float m_Temperature{}, m_Voltage{}, m_Current{}, m_IdleCurrent{}, m_Charge{};

    bool m_Completed = false, m_ReadyForNext  = false;

    float m_TemperatureValue[8] = { -10, 0, 10, 20, 30, 40, 50, 60 };
    float m_TemperatureVolts[8] = { 1.766, 1.405, 1.065, 0.779, 0.558, 0.395, 0.280, 0.200 };
};