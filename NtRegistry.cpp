// System Includes
#include "stdafx.h"

#include <windows.h>
#include <sddl.h>

// Local Includes
#include "ntregistry.h"
#include "utils.h"

NtDllLib NtRegistry::m_ntdlllib;


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
bool NtRegistry::create(HKEY hKey, const wchar_t* szSubKey, REGSAM samDesired)
{
    std::wstring sKey(getPath(hKey, szSubKey));
    return create(sKey.c_str(), NULL, samDesired);
}

bool NtRegistry::create(HKEY hKey, const wchar_t* szSubKey, SECURITY_ATTRIBUTES *sa, REGSAM samDesired, ULONG Attributes)
{
    std::wstring sKey(getPath(hKey, szSubKey));
    return create(sKey.c_str(), sa, samDesired, Attributes);
}

bool NtRegistry::create(const wchar_t* szKey, SECURITY_ATTRIBUTES *sa, REGSAM samDesired, ULONG Attributes)
{
    close();

    m_sKey = szKey;

    UNICODE_STRING usKeyPath;
    RtlZeroMemory(&usKeyPath, sizeof(usKeyPath));
    m_ntdlllib.RtlInitUnicodeString(&usKeyPath, m_sKey.c_str());

    OBJECT_ATTRIBUTES ObjectAttributes;
    InitializeObjectAttributes(&ObjectAttributes, &usKeyPath, Attributes, NULL, (sa != NULL) ? sa->lpSecurityDescriptor : NULL);

    return m_ntdlllib.NtCreateKey(&m_hKey, KEY_ALL_ACCESS, &ObjectAttributes, 0, NULL, REG_OPTION_NON_VOLATILE, NULL) == STATUS_SUCCESS;
}

bool NtRegistry::open(HKEY hKey, const wchar_t* szSubKey, REGSAM samDesired)
{
    BOOL bIsHidden = FALSE;
    std::wstring sKey(getPath(hKey, szSubKey));
    return open(sKey.c_str(), bIsHidden, samDesired);
}

bool NtRegistry::open(HKEY hKey, const wchar_t* szSubKey, BOOL &bIsHidden, REGSAM samDesired)
{
    std::wstring sKey(getPath(hKey, szSubKey));
    return open(sKey.c_str(), bIsHidden, samDesired);
}

bool NtRegistry::open(const wchar_t* szKey, BOOL &bIsHidden, REGSAM samDesired)
{
    close();

    m_sKey = szKey;

    UNICODE_STRING usKeyPath;
    RtlZeroMemory(&usKeyPath, sizeof(usKeyPath));
    m_ntdlllib.RtlInitUnicodeString(&usKeyPath, m_sKey.c_str());

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

        std::wstring baseReg = L"\\" + tokens[0] + L"\\" + tokens[1] + L"\\" + tokens[2];

        RtlZeroMemory(&usKeyPath, sizeof(usKeyPath));
        m_ntdlllib.RtlInitUnicodeString(&usKeyPath, baseReg.c_str());
        InitializeObjectAttributes(&ObjectAttributes, &usKeyPath, OBJ_CASE_INSENSITIVE | OBJ_OPENLINK, NULL, NULL);

        retVal = m_ntdlllib.NtOpenKey(&htemp, samDesired, &ObjectAttributes);
        if(STATUS_SUCCESS == retVal) {

            for(size_t idx = 3; idx < tokens.size(); idx++ ) {

                UNICODE_STRING usKeyPath;
                RtlZeroMemory(&usKeyPath, sizeof(usKeyPath));
                m_ntdlllib.RtlInitUnicodeString(&usKeyPath, tokens[idx].c_str());

                OBJECT_ATTRIBUTES ObjectAttributes;
                InitializeObjectAttributes(&ObjectAttributes, &usKeyPath, OBJ_CASE_INSENSITIVE | OBJ_OPENLINK, htemp, NULL);

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

bool NtRegistry::setValue(const wchar_t* lpValueName, const std::wstring &value)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

    return m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_SZ, (PVOID)value.c_str(), ((int)value.length() + 1) * sizeof(wchar_t)) == STATUS_SUCCESS;
}

bool NtRegistry::setValue(const wchar_t* lpValueName, DWORD nValue)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

    return m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_DWORD, (PVOID)nValue, sizeof(nValue)) == STATUS_SUCCESS;
}

bool NtRegistry::setValue(const wchar_t* lpValueName, BOOL nValue)
{
    return setValue(lpValueName, (DWORD)nValue);
}

bool NtRegistry::setValue(const wchar_t* lpValueName, const unsigned char * pValue, unsigned long nSize)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

    return m_ntdlllib.NtSetValueKey(m_hKey, &usValueName, NULL, REG_BINARY, (PVOID)pValue, nSize) == STATUS_SUCCESS;
}

bool NtRegistry::setValue(const wchar_t* lpValueName, const char * pValue, unsigned long nSize)
{
    return setValue(lpValueName, (const unsigned char *)pValue, nSize);
}

bool NtRegistry::setValue(const wchar_t* lpValueName, const std::vector<std::wstring> & strings)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

bool NtRegistry::getValue(const wchar_t * lpValueName, std::wstring & value)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

bool NtRegistry::getValue(const wchar_t* lpValueName, DWORD & nValue)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

bool NtRegistry::getValue(const wchar_t* lpValueName, BOOL & nValue)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

bool NtRegistry::getValue(const wchar_t* lpValueName, unsigned char * nValue, unsigned long nSize)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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
bool NtRegistry::getValue(const wchar_t* lpValueName, char * nValue, unsigned long nSize)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

bool NtRegistry::getValue(const wchar_t* lpValueName, std::vector<std::wstring> &strings)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

bool NtRegistry::deleteValue(const wchar_t* lpValueName)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

            if(reg.open(m_sKey.c_str(), bIsHidden, KEY_ALL_ACCESS))
                bSuccess = bSuccess && reg.deleteSubKey(iterSubKey->c_str());
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
bool NtRegistry::deleteSubKey(const wchar_t* lpSubKey)
{
    std::wstring sKey(m_sKey);
    std::wstring sSubKey(lpSubKey);
    if(m_sKey.at(m_sKey.length() - 1) != L'\\' && sSubKey.at(sSubKey.length() - 1) != L'\\')
        sKey += L"\\";
    sKey += sSubKey;

    bool bSuccess = false;

    NtRegistry ntreg;
    BOOL bIsHidden = FALSE;

    if(ntreg.open(sKey.c_str(), bIsHidden, KEY_ALL_ACCESS))
        bSuccess = ntreg.deleteSubKey();
    return bSuccess;
}

bool NtRegistry::getValueSize(const wchar_t * lpValueName, unsigned long & nSize)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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

bool NtRegistry::getValueType(const wchar_t *lpValueName, unsigned long & nType)
{
    UNICODE_STRING usValueName;
    m_ntdlllib.RtlInitUnicodeString(&usValueName, lpValueName);

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
            std::vector<wchar_t> valueName(nValueNameLength);

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

                        std::wstring sName(info->Name, info->NameLength / sizeof(wchar_t));
                        strings.push_back(sName);
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
            std::vector<wchar_t> valueName(nValueNameLength);

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

                        std::wstring sName(info->Name, info->NameLength/ sizeof(wchar_t));
                        strings.push_back(sName);
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

std::wstring NtRegistry::getPath(HKEY hKey, const wchar_t *lpSubKey)
{
    std::wstring sPath = getRootPath(hKey);
    if(lpSubKey[0] == L'\\')
    {
        sPath += lpSubKey;
    }
    else
    {
        sPath += L"\\";
        sPath += lpSubKey;
    }

    return sPath;
}

std::wstring NtRegistry::getPath(HKEY hKey, const wchar_t *lpSubKey, const int nSubKeyLength, int &nKeyLength)
{
    std::wstring sPath = getRootPath(hKey);
    if(lpSubKey[0] == L'\\')
    {
        nKeyLength = (int)sPath.length() + nSubKeyLength;
        sPath += lpSubKey;
    }
    else
    {
        sPath += L"\\";
        nKeyLength = (int)sPath.length() + nSubKeyLength;

        sPath += lpSubKey;
    }

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
