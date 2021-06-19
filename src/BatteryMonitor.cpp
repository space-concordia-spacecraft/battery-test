#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(MainWindow & w):m_Window(w){
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
    // TODO calculations for IM and TM values
    // Store results in m_BatteryA and m_BatteryB
//    std::cout << data.m_CellA_TM << std::endl;
//    std::cout << data.m_CellA_IM << std::endl;
//    std::cout << data.m_CellA_VM << std::endl;
//
//    std::cout << data.m_CellB_TM << std::endl;
//    std::cout << data.m_CellB_IM << std::endl;
//    std::cout << data.m_CellB_VM << std::endl;
//
//    std::cout << data.m_VRef << std::endl;
//    std::cout << data.m_Jig_TM << std::endl;

    if(m_BatteryA.isIdle()) {
        m_BatteryA.setIdleCurrent(data.m_CellA_IM);
    }

    if(m_BatteryB.isIdle()) {
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

void startCharging() {

}