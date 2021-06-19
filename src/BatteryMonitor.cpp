#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(MainWindow& w, SerialPort& port) : m_Window(w), m_ArduinoPort(port) {
    m_LabelAVoltage = m_Window.findChild<QLabel*>("cell_a_voltage");
    m_LabelACurrent = m_Window.findChild<QLabel*>("cell_a_current");
    m_LabelATemp = m_Window.findChild<QLabel*>("cell_a_temperature");
    m_LabelACharge = m_Window.findChild<QLabel*>("cell_a_charge");
    m_LabelAStage = m_Window.findChild<QLabel*>("cell_a_stage");

    m_LabelBVoltage = m_Window.findChild<QLabel*>("cell_b_voltage");
    m_LabelBCurrent = m_Window.findChild<QLabel*>("cell_b_current");
    m_LabelBTemp = m_Window.findChild<QLabel*>("cell_b_temperature");
    m_LabelBCharge = m_Window.findChild<QLabel*>("cell_b_charge");
    m_LabelBStage = m_Window.findChild<QLabel*>("cell_b_stage");
}

void BatteryMonitor::OnReceive(SerialData data) {
    // Store results in m_BatteryA and m_BatteryB

    if (m_BatteryA.isIdle()) {
        m_BatteryA.setIdleCurrent(data.m_CellA_IM);
    }

    if (m_BatteryB.isIdle()) {
        m_BatteryB.setIdleCurrent(data.m_CellB_IM);
    }

    m_BatteryA.setTemp(data.m_CellA_TM);
    m_BatteryA.setVoltage(data.m_CellA_VM);
    m_BatteryA.setCurrent(data.m_CellA_IM);

    m_BatteryB.setTemp(data.m_CellB_TM);
    m_BatteryB.setVoltage(data.m_CellB_VM);
    m_BatteryB.setCurrent(data.m_CellB_IM);

    this->m_JigTemperature = data.m_Jig_TM;
    this->m_VRef = data.m_VRef;

    static const int precision = 3;
    m_LabelAVoltage->setText(QString::number(m_BatteryA.getVolt(), 'g', precision));
    m_LabelATemp->setText(QString::number(m_BatteryA.getTemp(), 'g', precision));
    m_LabelACurrent->setText(QString::number(m_BatteryA.getCurrent(), 'g', precision));

    m_LabelBVoltage->setText(QString::number(m_BatteryB.getVolt(), 'g', precision));
    m_LabelBTemp->setText(QString::number(m_BatteryB.getTemp(), 'g', precision));
    m_LabelBCurrent->setText(QString::number(m_BatteryB.getCurrent(), 'g', precision));
}

void BatteryMonitor::Start() {
    m_Running = true;
    m_Thread = thread(&BatteryMonitor::Run, this);

    m_BatteryA.setState(Battery::CHARGE1);
    m_BatteryB.setState(Battery::IDLE0);

    m_BatteryA.setCompleted(false);
    m_BatteryB.setCompleted(false);

    //TODO: send command to arduino to charge A and idle B
    m_ArduinoPort.writeSerialPort(COMMAND_CHARGE_A);
}

void BatteryMonitor::Stop() {
    m_BatteryA.setState(Battery::IDLE0);
    m_BatteryB.setState(Battery::IDLE0);

    m_BatteryA.setCompleted(true);
    m_BatteryB.setCompleted(true);

    //TODO: send command to put both cells on idle

    m_Running = false;
    m_Thread.join();
}

void BatteryMonitor::Run() {
    auto currentMillisA = std::chrono::high_resolution_clock::now();
    auto currentMillisB = std::chrono::high_resolution_clock::now();

    while (m_Running && (!m_BatteryA.isCompleted() || !m_BatteryB.isCompleted())) {

        checkBattery(m_BatteryA, currentMillisA);
        checkBattery(m_BatteryB, currentMillisB);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void BatteryMonitor::checkBattery(Battery battery, std::chrono::steady_clock::time_point currentMillis) {
    if (battery.getGeneralState() == Battery::CHARGING && battery.getVolt() >= Battery::HIGHEST_VOLTAGE) {
        if (battery.getState() == Battery::CHARGE3) {
            battery.setCompleted(true);
        }

        battery.goNext();

    } else if (battery.getGeneralState() == Battery::DISCHARGING && battery.getVolt() <= Battery::LOWEST_VOLTAGE) {
        battery.goNext();
    } else {
        std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - currentMillis;
        if (elapsed.count() >= 600) {
            battery.goNext();
        }
    }
}