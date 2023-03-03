#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <ImGuizmo.h>
#include <implot.h>

#include "utils/battery_monitor.h"

namespace zeus {

    class Application {

    public:

        /**
        * Adds the Module to Hermes Main Window.
        * @param window
        */
        void AddWindow(UIWindow* window);

        /**
         * Creates Hermes and Runs the Entire Application
         * @return
         */
        int Run();

    private:
        GLFWwindow* m_Window;
        ImGuiContext* m_Context;
        vector<UIWindow*> m_Modules;

        /**
         * Creates the GLFW m_Context for Hermes Application
         * @return -1 if m_Context creation failed.
         */
        int SetupGLFW();

        /**
         * Creates the Context for the ImGUI m_Modules in Hermes
         */
        void SetupImGui();

        /**
         * Renders the List of Modules Present in the Module.
         */
        void RenderModules();

        /**
         * Creates the Dockspace, so that the windows can be docked in the Main Application.
         */
        void CreateDockspace();

        /**
         * Creates the Menu bar for the Main Application. This is also the way to interact with the Application.
         */
        void CreateMenuBar();

        /**
         * Polls the Events in the GLFW m_Context and renders the new changes.
         */
        void PollEvents();

        /**
         * Calls PollEvents to Update Windows and swaps GLFW buffers.
         */
        void UpdateWindows();

        /**
         * Sets the theme colors and fonts to be used by ImGui.
         */
        void SetupImGuiTheme();

    };

}
