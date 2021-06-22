#pragma once

#include <iostream>
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

    constexpr const static float LOWEST_CURRENT = 0.05f;

    explicit Battery(SerialPort& port) : m_ArduinoPort(port) {}

    int getGeneralStage() const {
        if(this->m_CurrentStage == IDLE0 || this->m_CurrentStage == IDLE1 || this->m_CurrentStage == IDLE2 || this->m_CurrentStage == IDLE3 || this->m_CurrentStage == IDLE4)
            return IDLE;
        else if(this->m_CurrentStage == CHARGE1 || this->m_CurrentStage == CHARGE2 || this->m_CurrentStage == CHARGE3)
            return CHARGING;
        else if(this->m_CurrentStage == DISCHARGE1 || this->m_CurrentStage == DISCHARGE2)
            return DISCHARGING;
        else
            return -1;
    };

    bool isIdle() const {
        return this->m_CurrentStage == IDLE0 || this->m_CurrentStage == IDLE1 || this->m_CurrentStage == IDLE2 || this->m_CurrentStage == IDLE3 || this->m_CurrentStage == IDLE4;
    }

    void charge() {
        m_ArduinoPort.writeSerialPort((std::string(COMMAND_CHARGE) + m_Letter + "\n").c_str());
    }

    void discharge() {
        m_ArduinoPort.writeSerialPort((std::string(COMMAND_DISCHARGE) + m_Letter + "\n").c_str());
    }

    void idle() {
        m_ArduinoPort.writeSerialPort((std::string(COMMAND_IDLE) + m_Letter + "\n").c_str());
    }

    void goNext() {
        m_CurrentStage = (m_CurrentStage + 1 ) % 9;

        int stage = this->getGeneralStage();

        if(stage == CHARGING) {
            charge();
        } else if(stage == DISCHARGING) {
            discharge();
        } else if (stage == IDLE) {
            idle();
        }
    }

    void setTemp(float data) {
        this->m_Temperature = interpolate(m_TemperatureVolts, m_TemperatureValue, data);
    }

    void setVoltage(float data) {
        this->m_Voltage = data;
        this->m_Charge = 100.0f * ( data - LOWEST_VOLTAGE )/( HIGHEST_VOLTAGE - LOWEST_VOLTAGE );
    }

    void setCurrent(float data) {
        this->m_Current = ( data - this->m_IdleCurrent ) / 0.681f;
    }

    void setIdleCurrent(float data) {
        this->m_IdleCurrent = data;
    }

    void setStage(int stage) {
        this->m_CurrentStage = stage;
    }

    void setCompleted(bool completed) {
        this->m_Completed = completed;
    }

    void setReadyForNext(bool ready) {
        this->m_ReadyForNext = ready;
    }

    void setArduinoPort(SerialPort& port) {
        this->m_ArduinoPort = port;
    }

    void setLetter(const std::string& letter) {
        this->m_Letter = letter;
    }

    float getTemp() const {
        return this->m_Temperature;
    }

    std::string getLetter() const {
        return this->m_Letter;
    }

    std::string getStageText() const {
        if(this->getGeneralStage() == CHARGING) {
            return "Charging";
        } else if(this->getGeneralStage() == DISCHARGING) {
            return "Discharging";
        } else if(this->getGeneralStage() == IDLE) {
            return "Idle";
        } else {
            return "ERROR";
        }
    }

    float getVolt() const {
        return this->m_Voltage;
    }

    float getCurrent() const {
        return this->m_Current;
    }

    float getIdleCurrent() const {
        return this->m_IdleCurrent;
    }

    float getCharge() const {
        return this->m_Charge;
    }

    int getState() const {
        return this->m_CurrentStage;
    }

    bool isCompleted() const {
        return this->m_Completed;
    }

    bool isReadyForNext() const {
        return this->m_ReadyForNext;
    }

private:
    int m_CurrentStage = IDLE0;

    std::string m_Letter;
    SerialPort & m_ArduinoPort;

    float m_Temperature{}, m_Voltage{}, m_Current{}, m_IdleCurrent{}, m_Charge{};

    bool m_Completed = false, m_ReadyForNext  = false;

    float m_TemperatureValue[8] = { -10, 0, 10, 20, 30, 40, 50, 60 };
    float m_TemperatureVolts[8] = { 1.766, 1.405, 1.065, 0.779, 0.558, 0.395, 0.280, 0.200 };
};