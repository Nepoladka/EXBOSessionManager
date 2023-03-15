#pragma once
#include "ConfigFile.h"


#define ascii_to_lower(c_char) ((c_char >= 'A' && c_char <= 'Z') ? (c_char + 32) : c_char)


static __forceinline std::string ToLower(std::string str)
{
    std::string Result;

    for (auto& CurrentChar : str)
        Result.push_back(ascii_to_lower(CurrentChar));

    return Result;
}

static __forceinline std::vector<std::string> SplitString(std::string data, char delim)
{
    std::vector<std::string> Elems;
    std::string Item;
    std::stringstream StringStream(data);

    while (getline(StringStream, Item, delim))
        Elems.push_back(Item);

    return Elems;
}

static __forceinline std::string FormatString(const char* const format, ...)
{
    va_list ArgList;
    va_start(ArgList, format);

    char Text[512]{ 0 };
    vsnprintf(Text, sizeof(Text), format, ArgList);

    va_end(ArgList);

    return Text;
}

static __forceinline bool RegGetStringValue(HKEY hKey, LPCSTR subKey, LPCSTR valueName, std::string* _value)
{
    if (!_value) return false;

    HKEY HKey;
    if (RegOpenKeyExA(hKey, subKey, NULL, KEY_ALL_ACCESS, &HKey) != ERROR_SUCCESS) return false;

    CHAR Buffer[512]{ 0 };
    DWORD BufferSize = sizeof(Buffer);

    LSTATUS Status = RegQueryValueExA(HKey, valueName, NULL, NULL, (LPBYTE)Buffer, &BufferSize);

    bool Result = Status == ERROR_SUCCESS;
    if (Result) *_value = Buffer;

    RegCloseKey(HKey);

    return Result;
}
