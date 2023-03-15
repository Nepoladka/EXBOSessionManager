#include "Window.h"
#include "SessionManager.h"


GLFWwindow* Window::GlfwHandle = NULL;
ImVec2 Window::CurrentSize;

SessionManager* Manager = NULL;


__forceinline void DrawGUI(const ImVec2& windowSize)
{
    ImGui::SetNextWindowSize(windowSize);
    ImGui::SetNextWindowPos({0, 0}, ImGuiCond_FirstUseEver);
    ImGui::Begin("MainWindow", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);


    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::TreeNode("Sessions", "Saved sessions (%d)", Manager->SavedSessions.size()))
    {
        if (!Manager->CurrentSession.Name.empty())
        {
            ImGui::Selectable(Manager->CurrentSession.ToString("\t").c_str());

            if (ImGui::BeginPopupContextItem("Current"))
            {
                if (ImGui::MenuItem("Save"))
                {
                    Manager->SaveSessionToFile(Manager->CurrentSession);

                    Manager->LoadSessionsFromFile();
                }

                ImGui::EndPopup();
            }

            ImGui::Spacing();
            ImGui::Spacing();
        }

        bool Break = false;
        for (SIZE_T i = 0; i < Manager->SavedSessions.size(); i++)
        {
            auto Session = &Manager->SavedSessions[i];

            ImGui::Selectable(Session->ToString("\t").c_str());

            if (ImGui::BeginPopupContextItem())
            {
                ImGui::Text(Session->Name.c_str());

                ImGui::Separator();

                if (Break = ImGui::MenuItem("Remove"))
                    Manager->SavedSessions.erase(*(std::vector<AuthorizationSession>::const_iterator*) & Session);

                if (ImGui::MenuItem("Set as current"))
                    Manager->SetCurrentSession(*Session);

                ImGui::EndPopup();
            }

            if (Break) break;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Settings"))
    {
        if (ImGui::Button("Launch EXBO"))
        {
            static char LauncherPath[512]{ 0 };
            if (!*LauncherPath) strcpy(LauncherPath, (Manager->RootPath + "\\java\\bin\\ExboLauncher.exe").c_str());

            CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)(&system), (LPVOID)(LauncherPath), NULL, NULL));
        }

        if (ImGui::Button("Toggle console"))
        {
            static const auto ConsoleWindow = GetConsoleWindow();
            ShowWindow(ConsoleWindow, SW_SHOW * !IsWindowVisible(ConsoleWindow));
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Info"))
    {
        ImGui::Text("Powered by github.com/glfw/glfw\nand github.com/ocornut/imgui\ncreated by github.com/Nepoladka");

        ImGui::TreePop();
    }


    ImGui::End();
}

__forceinline void DrawingThreadBody()
{
    glfwPollEvents();

    glfwGetWindowSize(Window::GlfwHandle, (int*)&Window::CurrentSize.x, (int*)&Window::CurrentSize.y);
    Window::CurrentSize.x = (float)*(int*)&Window::CurrentSize.x;
    Window::CurrentSize.y = (float)*(int*)&Window::CurrentSize.y;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    DrawGUI(Window::CurrentSize);


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(Window::GlfwHandle);

    Sleep(25);
}


int main()
{
    Manager = new SessionManager();

    if (!Window::Initialize())
    {
        system("pause");

        return EXIT_FAILURE;
    }
    else
    {
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    }

    while (!glfwWindowShouldClose(Window::GlfwHandle))
    {
        DrawingThreadBody();
    }

    Window::Terminate();

    return EXIT_SUCCESS;
}
