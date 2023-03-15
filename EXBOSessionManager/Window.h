#pragma once
#include <windows.h>

#include <string>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#pragma comment(lib, "glfw3.lib")

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"


namespace Window
{
	static constexpr const char* Title = "EXBO Session Manager";
	static constexpr const struct { long x, y; }Size = { 600, 300 };
    static constexpr const bool StaticSize = true;

	extern GLFWwindow* GlfwHandle;
    extern ImVec2 CurrentSize;


	static __forceinline bool Initialize()
	{
        auto InitCode = glfwInit();
        if (!InitCode)
        {
            printf("GLFW initialize failed with code: %d\n", InitCode);
            return false;
        }

        glfwWindowHint(GLFW_RESIZABLE, !StaticSize);


        GlfwHandle = glfwCreateWindow(Size.x, Size.y, Title, NULL, NULL);
        if (!GlfwHandle)
        {
            printf("GLFW creating window failed!\n");
            return false;
        }


        glfwMakeContextCurrent(GlfwHandle);

        glfwSwapInterval(GLFW_DONT_CARE);



        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        static constexpr const ImWchar CharsRanges[] = { 0x0020, 0x00FF, 0x0400, 0x04FF, 0, };
        ImFontConfig FontCfg;
        FontCfg.PixelSnapH = FontCfg.OversampleH = FontCfg.OversampleV = 1;


        ImGuiIO& IO = ImGui::GetIO();

        IO.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 18, &FontCfg, CharsRanges); // AddFontFromMemoryTTF(font, 1, 13, &font_config, ranges);

        ImGui::StyleColorsDark();

        IO.IniFilename = NULL;

        ImGui_ImplGlfw_InitForOpenGL(GlfwHandle, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        return true;
	}

    static __forceinline void Terminate()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        glfwDestroyWindow(Window::GlfwHandle);
        glfwTerminate();
    }

}