#pragma once

#include <windows.h>
#include <string>

#include <vector>

#define		REG_VALUENAME_DEFAULT	L"(Default)"

#define		REG_MAX_VALUENAME		16383

class RegItem
{
public:
	RegItem(void);
	virtual ~RegItem(void);

	void set(const HKEY hKey, const std::wstring &sSubKey, const std::wstring &sValueName, const int nType = REG_SZ);

	std::wstring toString() const;
	static std::wstring typeToString(const int nType);

	void setKey(HKEY hKey);
	HKEY getKey() const;

	void setSubKey(const std::wstring &sSubKey);
	std::wstring getSubKey() const;

	void setValueName(const std::wstring &sValueName);
	std::wstring getValueName() const;

	void setType(const int nType);
	int getType() const;

private:
	HKEY m_hKey;
	std::wstring m_sSubKey;
	std::wstring m_sValueName;
	int m_nType;
};


#define		VEC_REGITEM		std::vector<RegItem>