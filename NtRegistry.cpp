// System Includes
#include "stdafx.h"

#include <windows.h>
#include <sddl.h>

// Local Includes
#include "ntregistry.h"
#include "utils.h"
#include <assert.h>

NtDllLib NtRegistry::m_ntdlllib;

void ToUnicodeString(std::wstring& value, UNICODE_STRING& usValue)
{
    RtlZeroMemory(&usValue, sizeof(usValue));

    // Don't use RtlInitUnicodeString (it calls wcslen on strings that maynot be null terminated)
    usValue.Length = (USHORT)(value.size() * sizeof(wchar_t));
    usValue.MaximumLength = (USHORT)(value.size() * sizeof(wchar_t));
    usValue.Buffer = (LPWSTR)value.data();
}

void ToUnicodeString(std::vector<wchar_t>& value, UNICODE_STRING& usValue)
{
    RtlZeroMemory(&usValue, sizeof(usValue));

    // Don't use RtlInitUnicodeString (it calls wcslen on strings that maynot be null terminated)
    usValue.Length = (USHORT)(value.size() * sizeof(wchar_t));
    usValue.MaximumLength = (USHORT)(value.size() * sizeof(wchar_t));
    usValue.Buffer = (LPWSTR)value.data();
}

NtRegistry::NtRegistry(void)
:m_hKey(NULL)
{
    if(!m_ntdlllib.isLoaded())
    {
        const int nMaxPath = MAX_PATH * 2;
        wchar_t szSystemPath[nMaxPath] = L"";
        GetSystemDirectoryW(szSystemPath, nMaxPath);

        m_ntdlllib.loadDll(szSystemPath);
    }
}

NtRegistry::~NtRegistry(void)
{
    close();
}

bool NtRegistry::create(HKEY hKey, const wchar_t* szSubKey, size_t cchSubKeyLength, REGSAM samDesired)
{
    std::wstring sKey(getPath(hKey, szSubKey, cchSubKeyLength));
    return create(sKey.c_str(), sKey.size(), NULL, samDesired);
}

bool NtRegistry::create(HKEY hKey, const wchar_t* szSubKey, size_t cchSubKeyLength, SECURITY_ATTRIBUTES *sa, REGSAM samDesired, ULONG Attributes)
{
    std::wstring sKey(getPath(hKey, szSubKey, cchSubKeyLength));
    return create(sKey.c_str(), sKey.size(), sa, samDesired, Attributes);
}

bool NtRegistry::create(const wchar_t* szKey, size_t cchSubKeyLength, SECURITY_ATTRIBUTES *sa, REGSAM samDesired, ULONG Attributes)
{
    close();
    m_sKey = std::wstring(szKey, cchSubKeyLength);

    UNICODE_STRING usKeyPath;
    ToUnicodeString(m_sKey, usKeyPath);

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usKeyPath, Attributes, NULL, (sa != NULL) ? sa->lpSecurityDescriptor : NULL);

    return m_ntdlllib.NtCreateKey(&m_hKey, KEY_ALL_ACCESS, &ObjectAttributes, 0, NULL, REG_OPTION_NON_VOLATILE, NULL) == STATUS_SUCCESS;
}

bool NtRegistry::open(HKEY hKey, const wchar_t* szSubKey, size_t cchSubKeyLength, REGSAM samDesired)
{
    BOOL bIsHidden = FALSE;
    std::wstring sKey(getPath(hKey, szSubKey, cchSubKeyLength));
    return open(sKey.c_str(), sKey.size(), bIsHidden, samDesired);
}

bool NtRegistry::open(HKEY hKey, const wchar_t* szSubKey, size_t cchSubKeyLength, BOOL &bIsHidden, REGSAM samDesired)
{
    std::wstring sKey(getPath(hKey, szSubKey, cchSubKeyLength));
    return open(sKey.c_str(), sKey.size(), bIsHidden, samDesired);
}

bool NtRegistry::open(const wchar_t* szKey, size_t cchKeyLength, BOOL &bIsHidden, REGSAM samDesired)
{
    close();

    m_sKey = std::wstring(szKey, cchKeyLength);

    UNICODE_STRING usKeyPath;
    ToUnicodeString(m_sKey, usKeyPath);

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usKeyPath, OBJ_CASE_INSENSITIVE | OBJ_OPENLINK, NULL, NULL);

#define     NT_STATUS_NAME_TOO_LONG     0xc0000106

    NTSTATUS retVal = m_ntdlllib.NtOpenKey(&m_hKey, samDesired, &ObjectAttributes);
    if(NT_STATUS_NAME_TOO_LONG == retVal) {

        // NASTY HACK
        // need to recursively open path
        std::vector<std::wstring> tokens;
        Utils::tokenize(m_sKey, tokens, L"\\/");
        HKEY htemp = 0;
        HKEY hcurrent = 0;

        std::wstring baseReg = L"\\";
        baseReg.append(tokens[0].c_str(), tokens[0].size());
        baseReg += L"\\";
        baseReg.append(tokens[1].c_str(), tokens[1].size());
        baseReg += L"\\";
        baseReg.append(tokens[2].c_str(), tokens[2].size());

        ToUnicodeString(m_sKey, usKeyPath);
        InitializeObjectAttributes(&ObjectAttributes, &usKeyPath, OBJ_CASE_INSENSITIVE | OBJ_OPENLINK, NULL, NULL);

        retVal = m_ntdlllib.NtOpenKey(&htemp, samDesired, &ObjectAttributes);
        if(STATUS_SUCCESS == retVal) {

            for(size_t idx = 3; idx < tokens.size(); idx++ ) {

                UNICODE_STRING usTokenPath;
                ToUnicodeString(tokens[idx], usTokenPath);

                OBJECT_ATTRIBUTES ObjectAttributes;
                InitializeObjectAttributes(&ObjectAttributes, &usTokenPath, OBJ_CASE_INSENSITIVE | OBJ_OPENLINK, htemp, NULL);

                retVal = m_ntdlllib.NtCreateKey(&hcurrent, samDesired, &ObjectAttributes, 0, NULL, REG_OPTION_NON_VOLATILE, NULL);
                if(STATUS_SUCCESS == retVal) {
                    m_hKey = hcurrent;
                    m_sKey = tokens[idx];
                }

                m_ntdlllib.NtClose(htemp);
                htemp = hcurrent;
            }
        }

    } else if(STATUS_SUCCESS != retVal)	{

        // Attempt to open hidden key
        usKeyPath.MaximumLength = usKeyPath.Length += 2;

        OBJECT_ATTRIBUTES HiddenObjectAttributes;
        InitializeObjectAttributes(&HiddenObjectAttributes, &usKeyPath, OBJ_CASE_INSENSITIVE | OBJ_OPENLINK, NULL, NULL);

        retVal = m_ntdlllib.NtOpenKey(&m_hKey, samDesired, &HiddenObjectAttributes);
        bIsHidden = (STATUS_SUCCESS == retVal);
    }

    return (STATUS_SUCCESS == retVal);
}

void NtRegistry::close()
{
    if(m_hKey)
        m_ntdlllib.NtClose(m_hKey);

    m_hKey = NULL;
    m_sKey = L"";
}

bool NtRegistry::setValue(const wchar_t* lpValueName, size_t cchValueLength, const std::wstring &value)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    return m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_SZ, (PVOID)value.c_str(), ((int)value.length() + 1) * sizeof(wchar_t)) == STATUS_SUCCESS;
}

bool NtRegistry::setValue(const wchar_t* lpValueName, size_t cchValueLength, DWORD nValue)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    return m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_DWORD, (PVOID)nValue, sizeof(nValue)) == STATUS_SUCCESS;
}

bool NtRegistry::setValue(const wchar_t* lpValueName, size_t cchValueLength, BOOL nValue)
{
    return setValue(lpValueName, cchValueLength, (DWORD)nValue);
}

bool NtRegistry::setValue(const wchar_t* lpValueName, size_t cchValueLength, const unsigned char * pValue, unsigned long nSize)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    return m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_BINARY, (PVOID)pValue, nSize) == STATUS_SUCCESS;
}

bool NtRegistry::setValue(const wchar_t* lpValueName, size_t cchValueLength, const char*pValue, unsigned long nSize)
{
    return setValue(lpValueName, cchValueLength, (const unsigned char *)pValue, nSize);
}

bool NtRegistry::setValue(const wchar_t* lpValueName, size_t cchValueLength, const std::vector<std::wstring> & strings)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    if (strings.size() == 0)
    {	// write double null to the registry if there is no strings
        wchar_t doubleNull[] = {0, 0};
        bSuccess = m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_MULTI_SZ, (PVOID)doubleNull, 4) == STATUS_SUCCESS;
    }
    else
    {
        //	find size of buffer required
        std::wstring::size_type nSize = 0;
        std::vector<std::wstring>::const_iterator iter;
        for (iter = strings.begin(); iter != strings.end(); iter++)
            nSize += iter->size() + 1;

        nSize += 1;			//	for double null terminate

        //	allocate buffer for size
        std::vector<wchar_t> vecData(nSize, 0);

        //	copy strings from array to the buffer
        wchar_t * pCurrent = &vecData[0];
        for (iter = strings.begin(); iter != strings.end(); iter++)
        {
            lstrcpyW(pCurrent, iter->c_str());
            pCurrent += iter->size() + 1;
        }
        *pCurrent = L'\0';

        bSuccess = m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_MULTI_SZ, (PVOID)&vecData[0], (ULONG)nSize * sizeof(wchar_t)) == STATUS_SUCCESS;
    }
    return bSuccess;
}

bool NtRegistry::getValue(const wchar_t * lpValueName, size_t cchValueLength, std::wstring & value)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            bSuccess = true;
            value = std::wstring((const wchar_t *)info->Data);
        }
    }

    return bSuccess;
}

bool NtRegistry::getValue(const wchar_t* lpValueName, size_t cchValueLength, DWORD & nValue)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            if(info->DataLength = sizeof(nValue))
            {
                bSuccess = true;
                nValue = (DWORD&)info->Data;
            }
        }
    }

    return bSuccess;
}

bool NtRegistry::getValue(const wchar_t* lpValueName, size_t cchValueLength, BOOL & nValue)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            if(info->DataLength == sizeof(nValue))
            {
                bSuccess = true;
                nValue = (BOOL)info->Data;
            }
        }
    }

    return bSuccess;
}

bool NtRegistry::getValue(const wchar_t* lpValueName, size_t cchValueLength, unsigned char * nValue, unsigned long nSize)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            if(info->DataLength == nSize)
            {
                bSuccess = true;
                memcpy(nValue, info->Data, nSize);
            }
        }
    }

    return bSuccess;
}
bool NtRegistry::getValue(const wchar_t* lpValueName, size_t cchValueLength, char * nValue, unsigned long nSize)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            if(info->DataLength == nSize)
            {
                bSuccess = true;
                memcpy(nValue, info->Data, nSize);
            }
        }
    }

    return bSuccess;
}

bool NtRegistry::getValue(const wchar_t* lpValueName, size_t cchValueLength, std::vector<std::wstring> &strings)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;
    DWORD nSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            bSuccess = true;
            strings.clear();

            DWORD nSize = info->DataLength;
            std::vector<char> vecValueData(nSize + 2, 0);
            memcpy(&vecValueData[0], info->Data, nSize);

            //	get each string from multi string buffer and add it to array
            vecValueData[nSize] = 0;			// I have allocated an extra byte
            vecValueData[nSize + 1] = 0;		// I have really enforced a double null

            DWORD nNullCount = 0;			// Check for double null termination
            DWORD nIndex = 0;
            for (; nIndex < nSize && nNullCount < 2; ++nIndex)
            {
                if (vecValueData[nIndex] != 0)
                {
                    std::wstring val = std::wstring((wchar_t*)&vecValueData[nIndex]);
                    strings.push_back(val);

                    nIndex += (int)val.length() * sizeof(wchar_t);

                    // Check for double null
                    if (vecValueData[nIndex + sizeof(wchar_t)] == 0)
                        break;

                    nNullCount = 0;
                }
                else
                {
                    nNullCount++;
                }
            }


        }
    }

    return bSuccess;
}

bool NtRegistry::deleteValue(const wchar_t* lpValueName, size_t cchValueLength)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    return m_ntdlllib.NtDeleteValueKey(m_hKey, &usValueName) == STATUS_SUCCESS;
}

bool NtRegistry::deleteSubKey()
{
    bool bSuccess = true;

    std::vector<std::wstring> vecSubKeys;
    if(listSubKeys(vecSubKeys))
    {
        // Delete list of folders
        std::vector<std::wstring>::const_iterator iterSubKey;
        for(iterSubKey = vecSubKeys.begin();
            iterSubKey != vecSubKeys.end(); ++iterSubKey)
        {
            NtRegistry reg;
            BOOL bIsHidden = FALSE;

            if(reg.open(m_sKey.c_str(), m_sKey.size(), bIsHidden, KEY_ALL_ACCESS))
                bSuccess = bSuccess && reg.deleteSubKey(iterSubKey->c_str(), iterSubKey->size());
        }
    }

    bSuccess = bSuccess && m_ntdlllib.NtDeleteKey(m_hKey) == STATUS_SUCCESS;
    if(bSuccess)
    {
        m_hKey = NULL;
        m_sKey = L"";
    }

    return bSuccess;
}
bool NtRegistry::deleteSubKey(const wchar_t* lpSubKey, size_t cchValueLength)
{
    std::wstring sKey(m_sKey);
    std::wstring sSubKey(lpSubKey, cchValueLength);
    if(m_sKey.at(m_sKey.length() - 1) != L'\\' && sSubKey.at(sSubKey.length() - 1) != L'\\')
        sKey += L"\\";
    sKey += sSubKey;

    bool bSuccess = false;
    NtRegistry ntreg;
    BOOL bIsHidden = FALSE;

    if(ntreg.open(sKey.c_str(), sKey.size(), bIsHidden, KEY_ALL_ACCESS))
        bSuccess = ntreg.deleteSubKey();
    return bSuccess;
}

bool NtRegistry::getValueSize(const wchar_t* lpValueName, size_t cchValueLength, unsigned long & nSize)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            nSize = info->DataLength;
            bSuccess = true;
        }
    }

    return bSuccess;
}

bool NtRegistry::getValueType(const wchar_t *lpValueName, size_t cchValueLength, unsigned long& nType)
{
    std::vector<wchar_t> valueName(cchValueLength);
    memcpy_s(valueName.data(), valueName.size() * sizeof(wchar_t), lpValueName, valueName.size() * sizeof(wchar_t));

    UNICODE_STRING usValueName;
    ToUnicodeString(valueName, usValueName);

    bool bSuccess = false;
    DWORD nReturnSize = 0;

    NTSTATUS nResult = m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<BYTE> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nAllocSize = nReturnSize;
        nReturnSize = 0;

        if(m_ntdlllib.NtQueryValueKey(m_hKey, &usValueName, KeyValuePartialInformation, &vecData[0], nAllocSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *) &vecData[0];

            nType = info->Type;
            bSuccess = true;
        }
    }

    return bSuccess;
}

bool NtRegistry::listValueNames(std::vector<std::wstring> & strings)
{
    DWORD nReturnSize = 0;
    NTSTATUS nResult = m_ntdlllib.NtQueryKey(m_hKey, KeyFullInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nSize = nReturnSize;
        nReturnSize = 0;
        if(m_ntdlllib.NtQueryKey(m_hKey, KeyFullInformation, &vecData[0], nSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_FULL_INFORMATION *info = (KEY_FULL_INFORMATION *)&vecData[0];

            DWORD nValueNameIndex = 0;
            const int nValueNameLength = info->MaxValueNameLen + 1;
            std::vector<wchar_t> valueName(nValueNameLength * sizeof(wchar_t));

            for (nValueNameIndex = 0; nValueNameIndex < info->Values; nValueNameIndex++)
            {
                nSize = 0;
                nReturnSize = 0;

                m_ntdlllib.NtEnumerateValueKey(m_hKey, nValueNameIndex, KeyValueBasicInformation, NULL, 0, &nReturnSize);
                if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
                {
                    std::vector<char> vecDataValue(nReturnSize * sizeof(wchar_t), 0);

                    nSize = nReturnSize;
                    nReturnSize = 0;

                    if(m_ntdlllib.NtEnumerateValueKey(m_hKey, nValueNameIndex, KeyValueBasicInformation,
                        &vecDataValue[0], nSize, &nReturnSize) == STATUS_SUCCESS)
                    {
                        KEY_VALUE_BASIC_INFORMATION *info = (KEY_VALUE_BASIC_INFORMATION *) &vecDataValue[0];
                        strings.emplace_back(info->Name, info->NameLength/sizeof(wchar_t));
                    }
                }
            }
        }
    }

    return (!strings.empty());
}

bool NtRegistry::listSubKeys(std::vector<std::wstring> & strings)
{
    DWORD nReturnSize = 0;
    NTSTATUS nResult = m_ntdlllib.NtQueryKey(m_hKey, KeyFullInformation, NULL, 0, &nReturnSize);
    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nSize = nReturnSize;
        nReturnSize = 0;
        if(m_ntdlllib.NtQueryKey(m_hKey, KeyFullInformation, &vecData[0], nSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_FULL_INFORMATION *info = (KEY_FULL_INFORMATION *)&vecData[0];

            DWORD nValueNameIndex = 0;
            const int nValueNameLength = info->MaxNameLen + 1;
            std::vector<wchar_t> valueName(nValueNameLength * sizeof(wchar_t));

            for (nValueNameIndex = 0; nValueNameIndex < info->SubKeys; nValueNameIndex++)
            {
                nSize = 0;
                nReturnSize = 0;

                m_ntdlllib.NtEnumerateKey(m_hKey, nValueNameIndex, KeyBasicInformation, NULL, 0, &nReturnSize);
                if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
                {
                    std::vector<char> vecDataValue(nReturnSize * sizeof(wchar_t), 0);

                    nSize = nReturnSize;
                    nReturnSize = 0;

                    if(m_ntdlllib.NtEnumerateKey(m_hKey, nValueNameIndex, KeyBasicInformation,
                        &vecDataValue[0], nSize, &nReturnSize) == STATUS_SUCCESS)
                    {
                        KEY_BASIC_INFORMATION *info = (KEY_BASIC_INFORMATION *) &vecDataValue[0];
                        strings.emplace_back(info->Name, info->NameLength/sizeof(wchar_t));
                    }
                }
            }
        }
    }

    return (!strings.empty());
}

int NtRegistry::getSubKeysCount()
{
    int nCount = 0;
    DWORD nReturnSize = 0;
    NTSTATUS nResult = m_ntdlllib.NtQueryKey(m_hKey, KeyFullInformation, NULL, 0, &nReturnSize);

    if(nResult == STATUS_BUFFER_OVERFLOW || nResult == STATUS_BUFFER_TOO_SMALL)
    {
        std::vector<char> vecData(nReturnSize * sizeof(wchar_t), 0);

        DWORD nSize = nReturnSize;
        nReturnSize = 0;
        if(m_ntdlllib.NtQueryKey(m_hKey, KeyFullInformation, &vecData[0], nSize, &nReturnSize) == STATUS_SUCCESS)
        {
            KEY_FULL_INFORMATION *info = (KEY_FULL_INFORMATION *)&vecData[0];
            nCount = info->SubKeys;
        }
    }

    return nCount;
}

std::wstring NtRegistry::getRootPath(HKEY hKey)
{
    std::wstring sRootPath;
    if (hKey == HKEY_LOCAL_MACHINE)
    {
        sRootPath = L"\\Registry\\Machine";
    }
    else if (hKey == HKEY_CLASSES_ROOT)
    {
        sRootPath = L"\\Registry\\Machine\\SOFTWARE\\Classes";
    }
    else if (hKey == HKEY_CURRENT_CONFIG)
    {
        sRootPath = L"\\Registry\\Machine\\System\\CurrentControlSet\\Hardware Profiles\\Current";
    }
    else if (hKey == HKEY_USERS)
    {
        sRootPath = L"\\Registry\\User";
    }
    else if (hKey == HKEY_CURRENT_USER)
    {
        // Initialise the current user sid
        if(m_sCurrentUserSID.empty())
            m_sCurrentUserSID = getCurrentUserSid();

        sRootPath = L"\\Registry\\User\\" + m_sCurrentUserSID + L"";
    }

    return sRootPath;
}

std::wstring NtRegistry::getPath(HKEY hKey, const wchar_t *lpSubKey, size_t cchSubKeyLength)
{
    std::wstring sPath = getRootPath(hKey);
    if(lpSubKey[0] != L'\\') {
        sPath += L"\\";
    }
    sPath += std::wstring(lpSubKey, cchSubKeyLength);

    return sPath;
}

std::wstring NtRegistry::getPath(HKEY hKey, const wchar_t *lpSubKey, size_t cchSubKeyLength, size_t &cchKeyLength)
{
    std::wstring sPath = getRootPath(hKey);
    if(sPath[sPath.length() - 1] != L'\\' && lpSubKey[0] != L'\\') {
        sPath += L"\\";
    }

    cchKeyLength = sPath.length() + cchSubKeyLength;
    sPath += std::wstring(lpSubKey, cchSubKeyLength);
    return sPath;
}

std::wstring NtRegistry::getCurrentUserSid()
{
    std::wstring sUserSid;
    HANDLE hToken = NULL;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        DWORD nReturnSize = 0;
        GetTokenInformation(hToken, TokenUser, NULL, 0, &nReturnSize);

        if(nReturnSize != 0)
        {
            DWORD nSize = nReturnSize;
            nReturnSize = 0;

            std::vector<char> vecData(nSize, 0);

            if(GetTokenInformation(hToken, TokenUser, &vecData[0], nSize, &nReturnSize) != 0)
            {
                TOKEN_USER *pTU = (TOKEN_USER *) &vecData[0];
                LPWSTR lpSid = 0;
                if(ConvertSidToStringSidW(pTU->User.Sid, &lpSid))
                {
                    sUserSid = lpSid;
                    LocalFree(lpSid);
                }
            }
        }

        CloseHandle(hToken);
    }

    return sUserSid;
}
