#pragma once

namespace zeus {

    class UIWindow {

    public:
        virtual void RenderGUI() = 0;
        virtual string GetName() const = 0;
    };

}