#pragma once

struct Battery {
    // Charge, Discharge, Charge, Discharge, Charge
    const int CHARGE1 = 0;
    const int DISCHARGE1 = 1;
    const int CHARGE2 = 2;
    const int DISCHARGE2 = 3;
    const int CHARGE3 = 4;

    int currentState = CHARGE1;

    float temp, voltage, current;

    bool isCharging() {
        return current > 0;
    };

    void goNext() {
        currentState = currentState + 1 % 5;
    }
};