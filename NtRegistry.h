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

	bool create(HKEY hKey, const wchar_t* szSubKey, REGSAM samDesired = KEY_ALL_ACCESS);
    bool create(HKEY hKey, const wchar_t* szSubKey, SECURITY_ATTRIBUTES *sa, REGSAM samDesired = KEY_ALL_ACCESS, ULONG ATTRIBUTES = OBJ_CASE_INSENSITIVE | OBJ_OPENLINK);

	bool open(HKEY hKey, const wchar_t* szSubKey, REGSAM samDesired = KEY_READ);
	bool open(HKEY hKey, const wchar_t* szSubKey, BOOL &bIsHidden, REGSAM samDesired = KEY_READ);

	void close();

	bool setValue(const wchar_t* lpValueName, const std::wstring & value);
	bool setValue(const char* lpValueName, const std::string & value);
	bool setValue(const wchar_t* lpValueName, DWORD nValue);
	bool setValue(const wchar_t* lpValueName, BOOL nValue);
	bool setValue(const wchar_t* lpValueName, const unsigned char * pValue, unsigned long nSize);
	bool setValue(const wchar_t* lpValueName, const char * pValue, unsigned long nSize);
	bool setValue(const wchar_t* lpValueName, const std::vector<std::wstring> & strings);

	bool getValue(const wchar_t * lpValueName, std::wstring & value);
	bool getValue(const char* lpValueName, std::string& value);
	bool getValue(const wchar_t* lpValueName, DWORD & nValue);
	bool getValue(const wchar_t* lpValueName, BOOL & nValue);
	bool getValue(const wchar_t* lpValueName, unsigned char * nValue, unsigned long nSize);
	bool getValue(const wchar_t* lpValueName, char * nValue, unsigned long nSize);
	bool getValue(const wchar_t* lpValueName, std::vector<std::wstring> &strings);

	bool getValueType(const wchar_t *lpValueName, unsigned long & nType);
	bool getValueSize(const wchar_t *lpValueName, unsigned long & nSize);

	bool deleteValue(const wchar_t* lpValueName);
	bool deleteSubKey();
	bool deleteSubKey(const wchar_t* lpSubKey);

	bool listValueNames(std::vector<std::wstring> & strings);

	bool listSubKeys(std::vector<std::wstring> & strings);
	int getSubKeysCount();

protected:
	bool create(const wchar_t* szKey, SECURITY_ATTRIBUTES *sa, REGSAM samDesired = KEY_ALL_ACCESS, ULONG Attributes = OBJ_CASE_INSENSITIVE | OBJ_OPENLINK);
	bool open(const wchar_t* szKey, BOOL &bIsHidden, REGSAM samDesired = KEY_READ);

	std::wstring getRootPath(HKEY hKey);

	std::wstring getPath(HKEY hKey, const wchar_t *lpSubKey);
	std::wstring getPath(HKEY hKey, const wchar_t *lpSubKey, const int nSubKeyLength, int &nKeyLength);

	std::wstring getCurrentUserSid();

private:
	static NtDllLib m_ntdlllib;
	std::wstring m_sCurrentUserSID;

	HKEY m_hKey;
	std::wstring m_sKey;
};
