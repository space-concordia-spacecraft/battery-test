#pragma once

#include <iostream>
#include <chrono>
#include <vector>

#include "Utils.h"
#include "SerialPort.h"

#define COMMAND_CHARGE "charge_"

#define COMMAND_DISCHARGE "discharge_"

#define COMMAND_IDLE "idle_"

class Battery {
public:

    /// Identifier for the CHARGING state
    const static int CHARGING = 0;

    /// Identifier for the DISCHARGING state
    const static int DISCHARGING = 1;

    /// Identifier for the IDLE state
    const static int IDLE = 2;

    /// Lowest voltage allowed for the battery
    constexpr const static float LOWEST_VOLTAGE = 3.09f;

    /// Highest voltage allowed for the battery
    constexpr const static float HIGHEST_VOLTAGE = 4.2f;

    /// Lowest current allowed for the battery
    constexpr const static float LOWEST_CURRENT = 0.05f;

    /**
     * Constructor to initialize a battery object.
     * @param port - SerialPort object used to communicate to the Electronic Load
     */
    explicit Battery(SerialPort& port);

    /**
     * Getter to obtain the current state of the battery.
     * @return the identifier for the current state of the battery.
     */
    int GetState() const;

    /**
     * Getter to get the current sequence state of the battery.
     * @return the index of how far it is in the testing sequence.
     */
    int GetCurrentSequenceState() const;

    /**
     * Getter to obtain the duration of the current state.
     * @return time elapsed from the beginning of the state to now in seconds.
     */
    std::chrono::seconds GetStateDuration() const;

    /**
     * Getter to get a label depending on the state of the battery.
     * @return Current state of the battery in String format.
     */
    std::string GetSequenceStateLabel() const;

    /**
     * Updates the current state of the battery.
     */
    void CompleteState();

    /**
     * Function to determine whether or not the battery has completed the testing sequence.
     * @return true if battery completed the full sequence.
     */
    bool IsCompleted();

    /**
     * Function to determine whether the battery is ready to go to the next state.
     * @return true if the battery is ready to go to the next state.
     */
    bool IsReady();

    /**
     * Function to start testing the battery depending on the state of the battery.
     */
    void StartStateTest();

    /**
     * Function to tell the hardware to charge the battery.
     */
    void Charge();

    /**
     * Function to tell the hardware to discharge the battery.
     */
    void Discharge();

    /**
     * Function to tell the hardware to put the battery in idle mode.
     */
    void Idle();

    /**
     * Setter for the temperature of the battery. It takes in voltage and converts it to celsius.
     * @param volt - Voltage value for the temperature of the battery.
     */
    void SetTemp(float volt);

    /**
     * Setter for the voltage of the battery.
     * @param volt - voltage value for the current voltage of the battery.
     */
    void SetVoltage(float volt);

    /**
     * Setter for the current of the battery.
     * @param current - current value for the battery.
     */
    void SetCurrent(float current);

    /**
     * Setter for the idle current of the battery.
     * @param current - idle current for the battery.
     */
    void SetIdleCurrent(float current);

    /**
     * Setter for the letter of the battery.
     * @param letter - character letter to identify the battery.
     */
    void SetLetter(const std::string& letter);

    /**
     * Setter for the SerialPort object used to communicate with hardware.
     * @param port - SerialPort object.
     */
    void SetArduinoPort(SerialPort& port);

    /**
     * Setter for the ready state of the battery.
     * @param ready - true if the battery is ready to go to the next state.
     */
    void SetReady(bool ready);

    /**
     * Setter for the current step in the sequence.
     * @param index - integer index for the step of the sequence.
     */
    void SetCurrentSequenceStep(int index);

    /**
     * Resets the current state start time.
     */
    void ResetStateStartTime();

    /**
     * Returns the state start time.
     * @return time point of the state start time.
     */
    std::chrono::steady_clock::time_point GetStateStartTime();

    /**
     * Getter for the temperature of the battery.
     * @return the temperature of the battery in celsius.
     */
    float GetTemp() const;

    /**
     * Getter for the voltage of the battery.
     * @return the voltage of the battery.
     */
    float GetVoltage() const;

    /**
     * Getter for the current of the battery.
     * @return the current of the battery.
     */
    float GetCurrent() const;

    /**
     * Getter for the idle current of the battery.
     * @return the idle current of the battery.
     */
    float GetIdleCurrent() const;

    /**
     * Getter for the letter of the battery.
     * @return the letter of the battery used as an identifier.
     */
    std::string GetLetter() const;

private:
    /// Sequence for the testing of the battery.
    std::vector<int> m_Sequence = { IDLE, CHARGING, IDLE, DISCHARGING, IDLE, CHARGING, IDLE, DISCHARGING, IDLE, CHARGING, IDLE, DISCHARGING, IDLE, CHARGING, IDLE };

    /// Index to determine the current state of the battery.
    int m_CurrentSequenceStep = 0;

    /// Start time for the state.
    std::chrono::steady_clock::time_point m_StateStartTime = std::chrono::high_resolution_clock::now();

    /// Letter to identify the battery.
    std::string m_Letter;

    /// SerialPort object used to communicate with the battery test jig hardware.
    SerialPort & m_ArduinoPort;

    /// Temperature of the battery in celsius.
    float m_Temperature{};

    /// Voltage of the battery.
    float m_Voltage{};

    /// Current of the battery.
    float m_Current{};

    /// Idle current of the battery.
    float m_IdleCurrent{};

    /// Boolean to determine whether the battery is ready for the next state.
    bool m_Ready = false;

    /// Array containing the celsius values depending on the voltage to interpolate.
    float m_TemperatureValue[8] = { -10, 0, 10, 20, 30, 40, 50, 60 };

    /// Array containing the voltage values to find the respective celsius values by interpolation.
    float m_TemperatureVolts[8] = { 1.766, 1.405, 1.065, 0.779, 0.558, 0.395, 0.280, 0.200 };
};
