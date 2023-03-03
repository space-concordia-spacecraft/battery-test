#include "windows/general_window.h"

namespace zeus {

    GeneralWindow::GeneralWindow(std::string name, BatteryMonitor& batteryMonitor, SerialReceiver& receiver) : m_WindowName(std::move(name)), m_BatteryMonitor(batteryMonitor), m_SerialReceiver(receiver) {}

    void GeneralWindow::RenderGUI() {
        ImGui::Begin(GetName().c_str());
        ImGui::Text("%s", "COM Ports");
        ImGui::SameLine();
        if(ImGui::Combo("##", &m_SelectedCOMPortIndex, COM_PORTS, IM_ARRAYSIZE(COM_PORTS))) {
            m_SerialReceiver.SetArduinoPort(COM_PORTS[m_SelectedCOMPortIndex]);
        }
        ImGui::SameLine();
        if(ImGui::Button("Start")) {
            m_BatteryMonitor.Start();
        }
        ImGui::SameLine();
        if(ImGui::Button("Stop")) {
            m_BatteryMonitor.Stop();
        }
        ImGui::End();
    }

    std::string GeneralWindow::GetName() const {
        return m_WindowName;
    }
}