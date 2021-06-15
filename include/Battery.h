#pragma once

struct Battery {
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

    int currentState = CHARGE1;

    float temp, voltage, current;

    bool isCharging() {
        return current > 0;
    };

    void goNext() {
        currentState = currentState + 1 % 9;
    }
};