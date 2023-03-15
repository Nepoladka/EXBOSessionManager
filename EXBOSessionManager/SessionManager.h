#pragma once
#include "Helpers.h"


static constexpr const char* DefaultFilePath = "saved-exbo-sessions.ini";


struct AuthorizationSession
{
	std::string Name;
	std::string Token;

	__forceinline std::string ToString(const std::string& separator = ":") { return Name + separator + Token; }
};


class SessionManager
{
public:
	std::string RootPath;
	std::string FilePath;

    AuthorizationSession CurrentSession;
	std::vector<AuthorizationSession> SavedSessions;


    __forceinline SessionManager(std::string filePath = std::string())
    {
        if (filePath.empty()) filePath = DefaultFilePath;

        GetRootPath();

        GetCurrentSession();

        LoadSessionsFromFile(FilePath = filePath);
    }


    __forceinline void GetRootPath()
    {
        RegGetStringValue(HKEY_CURRENT_USER, "SOFTWARE\\EXBO", "root", &RootPath);
    }

    __forceinline void GetCurrentSession()
    {
        RegGetStringValue(HKEY_CURRENT_USER, "SOFTWARE\\EXBO", "EXBO_LOGIN", &CurrentSession.Name);
        RegGetStringValue(HKEY_CURRENT_USER, "SOFTWARE\\EXBO", "EXBO_SESSION", &CurrentSession.Token);
    }

    __forceinline SIZE_T LoadSessionsFromFile(std::string path = std::string())
    {
        if (path.empty()) path = FilePath;

        SavedSessions.clear();

        auto FileLines = ConfigFile(path, true).GetLines();
        for (auto& Line : FileLines)
        {
            auto Parts = SplitString(Line, ':');
            if (Parts.size() != 2) continue;
            SavedSessions.push_back({ Parts[0], Parts[1] });
        }

        SavedSessions.shrink_to_fit();

        return SavedSessions.size();
    }

    __forceinline SIZE_T SaveSessionToFile(AuthorizationSession& session, std::string path = std::string())
    {
        if (path.empty()) path = FilePath;

        auto SessionData = session.ToString();

        auto File = ConfigFile(path, true);

        auto FileLines = File.GetLines();
        for (auto& Line : FileLines)
            if (Line == SessionData) return true;

        FileLines.push_back(SessionData);
        File.SetLines(FileLines);
    }

    __forceinline void SetCurrentSession(AuthorizationSession& session)
    {
        CurrentSession = session;

        RegSetKeyValueA(HKEY_CURRENT_USER, "SOFTWARE\\EXBO", "EXBO_LOGIN", REG_EXPAND_SZ, session.Name.data(), session.Name.size());
        RegSetKeyValueA(HKEY_CURRENT_USER, "SOFTWARE\\EXBO", "EXBO_SESSION", REG_EXPAND_SZ, session.Token.data(), session.Token.size());
    }

};
