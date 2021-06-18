#pragma once

#include "Battery.h"
#include "SerialReceiver.h"

class BatteryMonitor : SerialListener {

public:
    virtual void OnReceive(SerialData data) override final;
    float m_JigTemperature, m_VRef;

private:
    Battery m_BatteryA{}, m_BatteryB{};

};