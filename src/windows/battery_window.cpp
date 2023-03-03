#include "windows/battery_window.h"
#include "battery/battery.h"

namespace zeus {
    BatteryWindow::BatteryWindow(std::string name, Battery &battery) : m_WindowName(std::move(name)),
                                                                             m_Battery(battery) {}

    void BatteryWindow::RenderGUI() {
        ImGui::Begin(GetName().c_str());

        ImGui::BeginTable("Current Values", 2);

        AddTableRowFloat("Voltage", m_Battery.m_Voltage, "%0.3fV");
        AddTableRowFloat("Current", m_Battery.m_Current, "%0.3fA");
        AddTableRowFloat("Temperature", m_Battery.m_Temperature, "%0.2fC");

        AddTableRowInt("Charge Cycles Completed", m_Battery.ChargeCyclesCompleted);
        AddTableRowInt("Discharge Cycles Completed", m_Battery.DischargeCyclesCompleted);

        ImGui::TableNextColumn();
        ImGui::Text("%s", "Current Sequence Step");
        ImGui::TableNextColumn();
        ImGui::Text("%s", m_Battery.GetSequenceStateLabel().c_str());

        ImGui::TableNextColumn();
        ImGui::Text("%s", "Time on Step");
        ImGui::TableNextColumn();
        ImGui::Text("%llis", m_Battery.GetStateDuration().count());

        ImGui::EndTable();

        ImGui::End();
    }

    std::string BatteryWindow::GetName() const {
        return m_WindowName;
    }

    void BatteryWindow::AddTableRowFloat(const char *label, float value, const char *format) {
        ImGui::TableNextColumn();
        ImGui::Text("%s", label);

        ImGui::TableNextColumn();
        ImGui::Text(format, value);
    }

    void BatteryWindow::AddTableRowInt(const char *string, int value) {
        ImGui::TableNextColumn();
        ImGui::Text("%s", string);

        ImGui::TableNextColumn();
        ImGui::Text("%i", value);
    }
}