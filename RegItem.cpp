#include "stdafx.h"
#include "regitem.h"


RegItem::RegItem(void) 
    : m_hKey(NULL) 
{

}

RegItem::~RegItem(void) 
{

}

void RegItem::set(const HKEY hKey, const std::wstring &sSubKey, const std::wstring &sValueName, const int nType)
{
    m_hKey = hKey;
    m_sSubKey = sSubKey;
    m_sValueName = sValueName;
    m_nType = nType;
}

std::wstring RegItem::toString() const
{
    std::wstring s;
    if(m_hKey == HKEY_CLASSES_ROOT)
        s = L"HKEY_CLASSES_ROOT";
    else if(m_hKey == HKEY_CURRENT_USER)
        s = L"HKEY_CURRENT_USER";
    else if(m_hKey == HKEY_LOCAL_MACHINE)
        s = L"HKEY_LOCAL_MACHINE";
    else if(m_hKey == HKEY_USERS)
        s = L"HKEY_USERS";
    else if(m_hKey == HKEY_CURRENT_CONFIG)
        s = L"HKEY_CURRENT_CONFIG";

    if(!s.empty())
    {
        if(!m_sSubKey.empty())
            s += L"\\" + m_sSubKey;

        if(!m_sValueName.empty())
            s += L"\\" + m_sValueName;
    }

    return s;
}

std::wstring RegItem::typeToString(const int nType)
{
    std::wstring sTypeString;

    switch(nType)
    {
    case REG_NONE:
        sTypeString = L"REG_NONE";
        break;
    case REG_SZ:
        sTypeString = L"REG_SZ";
        break;
    case REG_EXPAND_SZ:
        sTypeString = L"REG_EXPAND_SZ";
        break;
    case REG_BINARY:
        sTypeString = L"REG_BINARY";
        break;
    case REG_DWORD:
        sTypeString = L"REG_DWORD";
        break;
    case REG_DWORD_BIG_ENDIAN:
        sTypeString = L"REG_DWORD_BIG_ENDIAN";
        break;
    case REG_LINK:
        sTypeString = L"REG_LINK";
        break;
    case REG_MULTI_SZ:
        sTypeString = L"REG_MULTI_SZ";
        break;
    case REG_RESOURCE_LIST:
        sTypeString = L"REG_RESOURCE_LIST";
        break;
    case REG_FULL_RESOURCE_DESCRIPTOR:
        sTypeString = L"REG_FULL_RESOURCE_DESCRIPTOR";
        break;
    case REG_RESOURCE_REQUIREMENTS_LIST:
        sTypeString = L"REG_RESOURCE_REQUIREMENTS_LIST";
        break;
    case REG_QWORD:
        sTypeString = L"REG_QWORD";
        break;
    default:
        sTypeString = L"UNKNOWN";
        break;
    }

    return sTypeString;
}

void RegItem::setKey(HKEY hKey) 
{ 
    m_hKey = hKey; 
}

HKEY RegItem::getKey() const 
{ 
    return m_hKey; 
}

void RegItem::setSubKey(const std::wstring &sSubKey) 
{ 
    m_sSubKey = sSubKey; 
}

std::wstring RegItem::getSubKey() const 
{ 
    return m_sSubKey; 
}

void RegItem::setValueName(const std::wstring &sValueName) 
{
    m_sValueName = sValueName; 
}

std::wstring RegItem::getValueName() const 
{ 
    return m_sValueName; 
}


void RegItem::setType(const int nType)
{
    m_nType = nType;
}

int RegItem::getType() const 
{
    return m_nType;
}
