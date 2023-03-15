#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <windows.h>


using namespace std;


class ConfigFile
{
private:
    struct section
    {
        string key;
        string value;
    };

    bool LinesOnly;
public:
    ConfigFile(string filePath, bool linesOnly = false)
    {
        LinesOnly = linesOnly;

        if (Open(filePath))
        {
            Load();

            Close();
        }

    }

    ConfigFile(char* data, bool linesOnly = false)
    {
        LinesOnly = linesOnly;

        if(data)
            Load(data);
    }

    ~ConfigFile()
    {
        if (FilePath.empty()) return;

        if (Open(FilePath))
        {
            Save();

            Close();
        }

    }


    bool GetString(string key, string* _value)
    {
        section* KeySection = GetSectionByKey(key);

        if (KeySection)
            *_value = KeySection->value;

        return KeySection;
    }

    void GetBool(string key, bool* _value)
    {
        string Value;

        if (GetString(key, &Value))
            *_value = (Value == "1" || Value == "true");
    }

    void GetInt32(string key, __int32* _value)
    {
        string Value;

        if (GetString(key, &Value))
            sscanf_s(Value.c_str(), "%d", _value);
    }

    void GetUInt32(string key, unsigned __int32* _value)
    {
        string Value;

        if (GetString(key, &Value))
            sscanf_s(Value.c_str(), "%x", _value);
    }

    void GetInt64(string key, __int64* _value)
    {
        string Value;

        if (GetString(key, &Value))
            sscanf_s(Value.c_str(), "%lld", _value);
    }

    void GetUInt64(string key, unsigned __int64* _value)
    {
        string Value;

        if (GetString(key, &Value))
            sscanf_s(Value.c_str(), "%llx", _value);
    }

    void GetFloat(string key, float* _value)
    {
        string Value;

        if (GetString(key, &Value))
            sscanf_s(Value.c_str(), "%f", _value);
    }

    void GetDouble(string key, double* _value)
    {
        string Value;

        if (GetString(key, &Value))
            sscanf_s(Value.c_str(), "%lf", _value);
    }


    void SetString(string key, string value)
    {
        section* Section = GetSectionByKey(key);
        if (Section)
        {
            Section->value = value;

            return;
        }

        Sections.push_back({ key, value });
    }

    void SetBool(string key, bool* value)
    {
        SetString(key, *value ? "1" : "0");
    }

    void SetInt32(string key, __int32* value)
    {
        char ValueStr[256]{ 0 };
        snprintf(ValueStr, sizeof(ValueStr), "%d", value ? *value : NULL);
        SetString(key, ValueStr);
    }

    void SetUInt32(string key, unsigned __int32* value)
    {
        char ValueStr[256]{ 0 };
        snprintf(ValueStr, sizeof(ValueStr), "%x", value ? *value : NULL);
        SetString(key, ValueStr);
    }

    void SetInt64(string key, __int64* value)
    {
        char ValueStr[256]{ 0 };
        snprintf(ValueStr, sizeof(ValueStr), "%lld", value ? *value : NULL);
        SetString(key, ValueStr);
    }

    void SetUInt64(string key, unsigned __int64* value)
    {
        char ValueStr[256]{ 0 };
        snprintf(ValueStr, sizeof(ValueStr), "%llx", value ? *value : NULL);
        SetString(key, ValueStr);
    }

    void SetFloat(string key, float* value)
    {
        char ValueStr[256]{ 0 };
        snprintf(ValueStr, sizeof(ValueStr), "%f", value ? *value : NULL);
        SetString(key, ValueStr);
    }

    void SetDouble(string key, double* value)
    {
        char ValueStr[256]{ 0 };
        snprintf(ValueStr, sizeof(ValueStr), "%lf", value ? *value : NULL);
        SetString(key, ValueStr);
    }

    vector<string> GetLines()
    {
        return Lines;
    }

    void SetLines(vector<string>& lines)
    {
        Lines = lines;
    }


    void Save(char* data)
    {
        string Data;

        if (LinesOnly)
        {
            for (auto& Line : Lines)
                Data += Line + "\n";
        }
        else
        {
            for (auto& Section : Sections)
                Data += Section.key + ": " + Section.value + "\n";
        }

        strcpy(data, Data.data());
    }

private:
    HANDLE FileHandle;
    string FilePath;
    vector<string> Lines;
    vector<section> Sections;


    vector<string> SplitString(string data, char delim)
    {
        string Item;
        vector<string> Elems;

        stringstream StringStream(data);

        while (getline(StringStream, Item, delim))
            Elems.push_back(Item);

        return Elems;
    }


    bool Open(string filePath)
    {
        FilePath = filePath;

        FileHandle = CreateFileA(FilePath.c_str(), GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        return FileHandle != INVALID_HANDLE_VALUE;
    }

    bool Create(string filePath)
    {
        FilePath = filePath;

        FileHandle = CreateFileA(FilePath.c_str(), GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

        return FileHandle != INVALID_HANDLE_VALUE;
    }

    void Close()
    {
        if (FileHandle != INVALID_HANDLE_VALUE)
            CloseHandle(FileHandle);

        FileHandle = INVALID_HANDLE_VALUE;
    }

    void Load()
    {
        SIZE_T FileSize = GetFileSize(FileHandle, NULL);
        if (!FileSize)
            return;

        char* FileData = new char[FileSize] { 0 };
        ReadFile(FileHandle, FileData, FileSize, NULL, NULL);

        Load(FileData);

        delete[] FileData;
    }

    void Load(char* data)
    {
        if (!(Lines = SplitString(data, '\n')).empty() && !LinesOnly)
            LoadSections(data);
    }

    void Save()
    {
        string Data;

        if (LinesOnly)
        {
            for (auto& Line : Lines)
                Data += Line + "\n";
        }
        else
        {
            for (auto& Section : Sections)
                Data += Section.key + ": " + Section.value + "\n";
        }

        WriteFile(FileHandle, Data.c_str(), Data.length(), NULL, NULL);
    }

    section* GetSectionByKey(string key)
    {
        for (auto& Section : Sections)
        {
            if (Section.key == key)
                return &Section;
        }

        return NULL;
    }

    bool SectionFromLine(string& line, section* _section)
    {
        auto Line = line.data();

        for (SIZE_T i = 0; i < line.size(); i++)
        {
            if (Line[i] != ':')
                continue;

            if (Line[i + 1] == '\0' || Line[i + 1] == '\n')
                return false;

            Line[i] = '\0';

            _section->key = Line;
            _section->value = (Line + i + 1 + (*(Line + i + 1) == ' '));

            return true;
        }

        return false;
    }

    bool LoadSections(string data)
    {
        for (auto& Line : Lines)
        {
            section CurrentSection;
            if(SectionFromLine(Line, &CurrentSection))
                Sections.push_back(CurrentSection);
        }

        return true;
    }

};
