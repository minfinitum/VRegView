// RegEdit.cpp : implementation file
//

#include "stdafx.h"
#include "regedit.h"
#include "regitem.h"
#include "ntregistry.h"

#include <string>
#include <sstream>
#include <iomanip>

// CRegEdit

IMPLEMENT_DYNAMIC(CRegEdit, CEdit)
CRegEdit::CRegEdit()
{
}

CRegEdit::~CRegEdit()
{
}


BEGIN_MESSAGE_MAP(CRegEdit, CEdit)
END_MESSAGE_MAP()



// CRegEdit message handlers

void CRegEdit::initialise()
{
    m_font.CreatePointFont(10, L"Courier");

    SetFont(&m_font, FALSE);
}

void CRegEdit::clear()
{
    SetWindowText(L"");
}

void CRegEdit::setRegValue(RegItem &item)
{
    clear();

    NtRegistry reg;
	std::wstring subKey(item.getSubKey());
    if(reg.open(item.getKey(), subKey.c_str(), subKey.size()))
    {
		std::wstring valueName(item.getValueName());
        unsigned long nSize = 0;
        if(reg.getValueSize(valueName.c_str(), valueName.size(), nSize) && nSize > 0)
        {
            unsigned long nType = 0;
            if(reg.getValueType(valueName.c_str(), valueName.size(), nType))
            {
                std::vector<char> vecData(nSize, 0);

                switch(nType)
                {
                case REG_SZ:
                case REG_EXPAND_SZ:
                    {
                        const int nMaxLen = (nSize / sizeof(wchar_t)) + 1; // add additional buffer
                        std::vector<wchar_t> vecData(nMaxLen, 0);

                        if(reg.getValue(valueName.c_str(), valueName.size(), (unsigned char *)&vecData[0], nSize))
                            setText(&vecData[0]);
                    }
                    break;

                case REG_DWORD:
                    {
                        DWORD nValue = 0;
                        if(reg.getValue(valueName.c_str(), valueName.size(), nValue))
                            setDWord(nValue);
                    }
                    break;

                case REG_MULTI_SZ:
                    {
                        std::vector<std::wstring> vecMultiString;
                        if(reg.getValue(valueName.c_str(), valueName.size(), vecMultiString))
                            setMultiString(vecMultiString);
                    }
                    break;

                case REG_DWORD_BIG_ENDIAN:
                case REG_QWORD:
                case REG_RESOURCE_REQUIREMENTS_LIST:
                case REG_FULL_RESOURCE_DESCRIPTOR:
                case REG_RESOURCE_LIST:
                case REG_NONE:
                case REG_LINK:
                case REG_BINARY:
                    {
                        std::vector<unsigned char> vecData(nSize, 0);
                        if(reg.getValue(valueName.c_str(), valueName.size(), &vecData[0], nSize))
                            setBinary(&vecData[0], nSize);
                    }
                    break;

                default:
                    break;
                }
            }
        }
        reg.close();
    }
}

void CRegEdit::setText(const std::wstring &sValue)
{
    SetWindowText(sValue.c_str());
}

void CRegEdit::setMultiString(const std::vector<std::wstring> &vecMultiString)
{
    std::wstringstream str;
    str.str(L"");

    std::vector<std::wstring>::const_iterator iterString;
    for(iterString = vecMultiString.begin(); iterString != vecMultiString.end(); ++iterString)
    {
        str << *iterString << L"\r\n";
    }

    SetWindowText(str.str().c_str());
}

void CRegEdit::setDWord(DWORD nValue)
{
    std::wstringstream str;
    str << L"0x" << std::hex << std::setw(8) << std::setfill(L'0') << nValue << L" (" << std::dec << nValue << L")";

    SetWindowText(str.str().c_str());
}

void CRegEdit::setBinary(unsigned char *pData, int nSize)
{
    const int nInterval = 16;

    std::wstringstream str;
    str.str(L"");

    wchar_t c = 0;
    int iterBin = 0;
    int textStartPos = 0;
    for(iterBin = 0; iterBin < nSize; iterBin++)
    {
        // add index
        if(iterBin % nInterval == 0)
            str << std::hex << std::setw(4) << std::setfill(L'0') << iterBin << L"  ";

        // add binary value
        str << std::hex << std::setw(2) << std::setfill(L'0') << pData[iterBin];

        // add text value - check if next value start of next interval
        if(iterBin != 0 && (iterBin + 1) % nInterval == 0)
        {
            str << L"  ";

            for(int iterText = textStartPos; iterText < iterBin + 1; iterText++)
            {
                c = pData[iterText];
                str << (wchar_t) (!(iswcntrl(c)) ? c : L'.');
            }

            // Index to start text on next interval
            textStartPos = iterBin + 1;

            str << L"\r\n";
        }
        else
        {
            str << L" ";
        }
    }

    if(iterBin % nInterval != 0)
    {
        // pad binary data
        int nNumBinaryValues = nInterval - (iterBin % nInterval);
        for(int iterPad = 0; iterPad < nNumBinaryValues; iterPad++)
            str << L"   ";

        str << L" ";

        // add text value
        for(int iterText = textStartPos; iterText < iterBin; iterText++)
        {
            c = pData[iterText];
            str << (wchar_t) (!(iswcntrl(c)) ? c : L'.');
        }

        str << L"\r\n";
    }

    if(str.str().length() > 0)
    {
        SetWindowText(str.str().c_str());
    }
}
