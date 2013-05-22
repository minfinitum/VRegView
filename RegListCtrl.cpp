// CRegListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "reglistctrl.h"
#include "ntregistry.h"
#include "resource.h"


#include <strsafe.h>

// CRegListCtrl
#define		VALUENAME_NAME			0 //L"Name"
#define		VALUENAME_TYPE			1 //L"Type"
#define		VALUENAME_SIZE			2 //L"Size"
#define		VALUENAME_MAX			3

IMPLEMENT_DYNAMIC(CRegListCtrl, CListCtrl)
CRegListCtrl::CRegListCtrl()
{
}

CRegListCtrl::~CRegListCtrl()
{
}


BEGIN_MESSAGE_MAP(CRegListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CRegListCtrl message handlers

void CRegListCtrl::initialise()
{
    // Initialise image list
    m_imageList.Create(16, 16, ILC_COLOR8 | ILC_MASK, 0, 2);

    CBitmap bmItemBin;
    bmItemBin.LoadBitmap(IDB_REG_BIN);
    m_imageList.Add(&bmItemBin, RGB(255, 0, 255));

    CBitmap bmItemText;
    bmItemText.LoadBitmap(IDB_REG_TEXT);
    m_imageList.Add(&bmItemText, RGB(255, 0, 255));

    SetImageList(&m_imageList, LVSIL_SMALL);

    // Update Expense List View
    CRect rect;
    GetClientRect(&rect);
    int nBarWidth = GetSystemMetrics(SM_CXVSCROLL);
    int nColumnWidth = (rect.right - rect.left - nBarWidth) / VALUENAME_MAX;

    InsertColumn(VALUENAME_NAME,		_T ("Name"), LVCFMT_LEFT, nColumnWidth);
    InsertColumn(VALUENAME_TYPE,		_T ("Type"), LVCFMT_LEFT, nColumnWidth);
    InsertColumn(VALUENAME_SIZE,		_T ("Size"), LVCFMT_LEFT, nColumnWidth);

    SetExtendedStyle(LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);

    m_keys.clear();
}

void CRegListCtrl::setValueNames(const RegItem &item)
{
    LockWindowUpdate();

    DeleteAllItems();
    m_keys.clear();

    NtRegistry reg;
    if(reg.open(item.getKey(), item.getSubKey().c_str()))
    {
        bool bDefaultAdded = false;

        std::vector<std::wstring> valueNames;
        if(reg.listValueNames(valueNames)) {

            std::wstring sName;
            std::wstring sData;
            unsigned long nType = REG_SZ;
            unsigned long nSize = 0;

            RegItem newItem;

            std::vector<std::wstring>::iterator iterValueName;
            for(iterValueName = valueNames.begin(); iterValueName != valueNames.end(); ++iterValueName)
            {
                sName = *iterValueName;
                if(sName.empty())
                {
                    sName = REG_VALUENAME_DEFAULT;
                    bDefaultAdded = true;
                }

                nType = REG_SZ;
                reg.getValueType(iterValueName->c_str(), nType);

                nSize = 0;
                reg.getValueSize(iterValueName->c_str(), nSize);	

                newItem.set(item.getKey(), item.getSubKey(), *iterValueName, nType);
                addItem(newItem, nSize);
            }
        }

        if(!bDefaultAdded)
        {
            RegItem newItem;
            newItem.set(item.getKey(), item.getSubKey(), REG_VALUENAME_DEFAULT, REG_SZ);
            addItem(newItem, 0);
        }
    }

    UnlockWindowUpdate();
}

void CRegListCtrl::addItem(const RegItem &item, const unsigned int nSize)
{
    LVITEM lvItem;
    ZeroMemory(&lvItem, sizeof(lvItem));

    int nNextItem = GetItemCount();

    const int nMaxLen = 512;
    wchar_t szValue[nMaxLen] = L"";

    // Save for sorting
    m_keys.push_back(item);

    // Name
    lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvItem.state = 0; 
    lvItem.stateMask = 0; 

    lvItem.iItem = nNextItem;
    lvItem.iSubItem = VALUENAME_NAME;
    lvItem.lParam = (LPARAM) m_keys.size() - 1;

    lvItem.iImage = 0;

    int nType = item.getType();
    if(nType == REG_SZ || nType == REG_MULTI_SZ || nType == REG_EXPAND_SZ)
        lvItem.iImage = 1;

    std::wstring sName = item.getValueName();
    if(!sName.empty())
        lvItem.pszText = (LPWSTR)sName.c_str();
    else 
        lvItem.pszText = L"(Default)";

    InsertItem(&lvItem);

    // Type
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0; 
    lvItem.stateMask = 0; 

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = VALUENAME_TYPE;

    StringCchPrintf(szValue, nMaxLen, L"%s", RegItem::typeToString(nType).c_str());
    lvItem.pszText = szValue;

    SetItem(&lvItem);

    // Size
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0; 
    lvItem.stateMask = 0; 

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = VALUENAME_SIZE;
    lvItem.lParam = (LPARAM) 0;

    StringCchPrintf(szValue, nMaxLen, L"%d", nSize);
    lvItem.pszText = szValue;

    SetItem(&lvItem);

    SortItems(CompareProc, (DWORD_PTR)this);

}

bool CRegListCtrl::getSelectedItem(std::wstring &sValueName)
{
    bool bSuccess = false;
    sValueName = L"";
    if(GetSelectedCount() > 0)
    {
        int nItem = GetNextItem(-1, LVNI_SELECTED);
        if(nItem != -1)
        {
            bSuccess = true;

            wchar_t szSelected[REG_MAX_VALUENAME];
            GetItemText(nItem, 0, szSelected, REG_MAX_VALUENAME);

            sValueName = szSelected;
            if(sValueName == REG_VALUENAME_DEFAULT)
                sValueName = L"";
        }
    }

    return bSuccess;
}

bool CRegListCtrl::getSelectedItemParam(RegItem &item)
{
    bool bSuccess = false;
    if(GetSelectedCount() > 0)
    {
        int nSelected = GetNextItem(-1, LVNI_SELECTED);
        if(nSelected != -1)
        {
            bSuccess = true;

            VEC_REGITEM::size_type nIndex = GetItemData(nSelected);
            if(nIndex >= 0 && nIndex < m_keys.size())
            {
                item = m_keys[nIndex];
            }
        }
    }

    return bSuccess;
}

int CALLBACK CRegListCtrl::CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CRegListCtrl *pListCtrl = (CRegListCtrl *)lParamSort;

    int nCompare = 0;
    std::wstring strItem1 = pListCtrl->m_keys[lParam1].getValueName();
    std::wstring strItem2 = pListCtrl->m_keys[lParam2].getValueName();

    if(strItem1 == REG_VALUENAME_DEFAULT)
    {
        nCompare = -1;		// ensure default is top most
    }
    else if(strItem2 == REG_VALUENAME_DEFAULT)
    {
        nCompare = 1;		// ensure default is top most
    }
    else
    {
        nCompare = CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, strItem1.c_str(), -1, strItem2.c_str(), -1);

        if(nCompare == CSTR_LESS_THAN)
            nCompare = -1;
        else if(nCompare == CSTR_EQUAL)
            nCompare = 0;
        else
            nCompare = 1;
    }

    return nCompare;
}
