#pragma once

#include "Battery.h"
#include "SerialReceiver.h"

class BatteryMonitor : SerialListener {

public:
    virtual void OnReceive(SerialData data) const override final;

private:
    Battery m_BatteryA{}, m_BatteryB{};

};