#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <ImGuizmo.h>
#include <implot.h>
#include <string>

namespace zeus {

    class UIWindow {

    public:
        virtual void RenderGUI() = 0;
        virtual string GetName() const = 0;
    };

}