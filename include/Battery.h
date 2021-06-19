#pragma once

#include "Utils.h"

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

    bool isCharging() {
        return m_Current > 0;
    };

    bool isIdle() {
        return this->m_CurrentState == IDLE0 || this->m_CurrentState == IDLE1 || this->m_CurrentState == IDLE2 || this->m_CurrentState == IDLE3 || this->m_CurrentState == IDLE4;
    }

    void goNext() {
        m_CurrentState = ( m_CurrentState + 1 ) % 9;
    }

    void setTemp(float data) {
        this->m_Temperature = interpolate(m_TemperatureVolts, m_TemperatureValue, data);
    }

    void setVoltage(float data) {
        this->m_Voltage = data;
    }

    void setCurrent(float data) {
        this->m_Current = ( data - this->m_idleCurrent ) / 0.681;
    }

    void setIdleCurrent(float data) {
        this->m_idleCurrent = data;
    }

    void setState(int state) {
        this->m_CurrentState = state;
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
        return this->m_idleCurrent;
    }

    int getState() {
        return this->m_CurrentState;
    }

private:
    int m_CurrentState = CHARGE1;

    float m_Temperature, m_Voltage, m_Current, m_idleCurrent;

    float m_TemperatureValue[8] = { -10, 0, 10, 20, 30, 40, 50, 60 };
    float m_TemperatureVolts[8] = { 1.766, 1.405, 1.065, 0.779, 0.558, 0.395, 0.280, 0.200 };
};