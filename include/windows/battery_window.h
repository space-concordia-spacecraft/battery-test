#pragma once

#include <battery/battery.h>
#include "core/ui_window.h"

namespace zeus {
    class BatteryWindow : public UIWindow {
    public:
        BatteryWindow(std::string name, Battery& battery);

        void RenderGUI() override;

        string GetName() const override;

    private:
        Battery &m_Battery;
        const std::string m_WindowName;

        void AddTableRowFloat(const char *label, float value, const char *format);

        void AddTableRowInt(const char *string, int value);
    };
}