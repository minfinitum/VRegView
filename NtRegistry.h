#pragma once

// System Includes
#include <windows.h>
#include <vector>

// Local Includes
#include "ntdlllib.h"

class NtRegistry
{
public:
    NtRegistry(void);
    virtual ~NtRegistry(void);

    bool create(HKEY hKey, const wchar_t* szSubKey, size_t cchSubKeyLength, SECURITY_ATTRIBUTES *sa, REGSAM samDesired = KEY_ALL_ACCESS, ULONG ATTRIBUTES = OBJ_CASE_INSENSITIVE | OBJ_OPENLINK);

    bool open(HKEY hKey, const wchar_t* szSubKey, size_t cchSubKeyLength, REGSAM samDesired = KEY_READ);
    bool open(HKEY hKey, const wchar_t* szSubKey, size_t cchSubKeyLength, BOOL &bIsHidden, REGSAM samDesired = KEY_READ);

    void close();

    bool setValue(const wchar_t* lpValueName, size_t cchValueLength, const std::wstring & value);
    bool setValue(const char* lpValueName, size_t cchValueLength, const std::string & value);
    bool setValue(const wchar_t* lpValueName, size_t cchValueLength, DWORD nValue);
    bool setValue(const wchar_t* lpValueName, size_t cchValueLength, BOOL nValue);
    bool setValue(const wchar_t* lpValueName, size_t cchValueLength, const unsigned char * pValue, unsigned long nSize);
    bool setValue(const wchar_t* lpValueName, size_t cchValueLength, const char * pValue, unsigned long nSize);
    bool setValue(const wchar_t* lpValueName, size_t cchValueLength, const std::vector<std::wstring> & strings);

    bool getValue(const wchar_t * lpValueName, size_t cchValueLength, std::wstring & value);
    bool getValue(const char* lpValueName, size_t cchValueLength, std::string& value);
    bool getValue(const wchar_t* lpValueName, size_t cchValueLength, DWORD & nValue);
    bool getValue(const wchar_t* lpValueName, size_t cchValueLength, BOOL & nValue);
    bool getValue(const wchar_t* lpValueName, size_t cchValueLength, unsigned char * nValue, unsigned long nSize);
    bool getValue(const wchar_t* lpValueName, size_t cchValueLength, char * nValue, unsigned long nSize);
    bool getValue(const wchar_t* lpValueName, size_t cchValueLength, std::vector<std::wstring> &strings);

    bool getValueInfo(const wchar_t *lpValueName, size_t cchValueLength, unsigned long& nSize, unsigned long& nType);

    bool deleteValue(const wchar_t* lpValueName, size_t cchValueLength);
    bool deleteSubKey();
    bool deleteSubKey(const wchar_t* lpSubKey, size_t cchValueLength);

    bool listValueNames(std::vector<std::wstring> & strings);

    bool listSubKeys(std::vector<std::wstring> & strings);
    int getSubKeysCount();

protected:
    bool create(const wchar_t* szKey, size_t cchKeyLength, SECURITY_ATTRIBUTES *sa, REGSAM samDesired = KEY_ALL_ACCESS, ULONG Attributes = OBJ_CASE_INSENSITIVE | OBJ_OPENLINK);
    bool open(const wchar_t* szKey, size_t cchKeyLength, BOOL &bIsHidden, REGSAM samDesired = KEY_READ);

    std::wstring getRootPath(HKEY hKey);

    std::wstring getPath(HKEY hKey, const wchar_t *lpSubKey, size_t cchSubKeyLength);
    std::wstring getPath(HKEY hKey, const wchar_t *lpSubKey, size_t cchSubKeyLength, size_t &cchKeyLength);

    std::wstring getCurrentUserSid();

private:
    static NtDllLib m_ntdlllib;
    std::wstring m_sCurrentUserSID;

    HKEY m_hKey;
    std::wstring m_sKey;
};
