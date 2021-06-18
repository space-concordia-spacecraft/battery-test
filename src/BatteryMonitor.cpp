#include "BatteryMonitor.h"

void BatteryMonitor::OnReceive(SerialData data) {
    // TODO calculations for IM and TM values
    // Store results in m_BatteryA and m_BatteryB
    std::cout << data.m_CellA_TM << std::endl;
    std::cout << data.m_CellA_IM << std::endl;
    std::cout << data.m_CellA_VM << std::endl;

    std::cout << data.m_CellB_TM << std::endl;
    std::cout << data.m_CellB_IM << std::endl;
    std::cout << data.m_CellB_VM << std::endl;

    std::cout << data.m_VRef << std::endl;
    std::cout << data.m_Jig_TM << std::endl;


    m_BatteryA.setTemp(data.m_CellA_TM);
    m_BatteryA.setVoltage(data.m_CellA_VM);
    m_BatteryA.setCurrent(data.m_CellA_IM);

    m_BatteryB.setTemp(data.m_CellB_TM);
    m_BatteryB.setVoltage(data.m_CellB_VM);
    m_BatteryB.setCurrent(data.m_CellB_IM);

    this->m_JigTemperature = data.m_Jig_TM;
    this->m_VRef = data.m_VRef;
}