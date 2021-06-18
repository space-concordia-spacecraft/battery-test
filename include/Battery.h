#pragma once

#include "Utils.h"

struct Battery {

private:
    // Charge, Discharge, Charge, Discharge, Charge
    const int CHARGE1 = 0;
    const int IDLE1 = 1;
    const int DISCHARGE1 = 2;
    const int IDLE2 = 3;
    const int CHARGE2 = 4;
    const int IDLE3 = 5;
    const int DISCHARGE2 = 6;
    const int IDLE4 = 7;
    const int CHARGE3 = 8;

    int m_CurrentState = CHARGE1;

    float m_Temperature, m_Voltage, m_Current, m_idleCurrent;

    float m_TemperatureValue[8] = { -10, 0, 10, 20, 30, 40, 50, 60 };
    float m_TemperatureVolts[8] = { 1.766, 1.405, 1.065, 0.779, 0.558, 0.395, 0.280, 0.200 };

public:
    bool isCharging() {
        return m_Current > 0;
    };

    bool isIdle() {
        return this->m_CurrentState == IDLE1 || this->m_CurrentState == IDLE2 || this->m_CurrentState == IDLE3 || this->m_CurrentState == IDLE4;
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
};