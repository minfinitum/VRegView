// CRegListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "reglistctrl.h"
#include "ntregistry.h"
#include "resource.h"
#include "ClipBoard.h"
#include "Utils.h"

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
    ON_NOTIFY_REFLECT(NM_RCLICK, &CRegListCtrl::OnNMRClick)
    ON_WM_CONTEXTMENU()
    ON_NOTIFY_REFLECT(NM_RCLICK, &CRegListCtrl::OnNMRClick)
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

void CRegListCtrl::refresh()
{
    setValueNames(m_current);
}

void CRegListCtrl::setValueNames(const RegItem &item)
{
    m_current = item;

    LockWindowUpdate();
    DeleteAllItems();
    m_keys.clear();

    NtRegistry reg;
    if(reg.open(item.getKey(), item.getSubKey().c_str(), item.getSubKey().length()))
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

                nSize = 0;
                nType = REG_SZ;
                reg.getValueInfo(iterValueName->c_str(), iterValueName->size(), nSize, nType);

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

    std::wstring sName = item.getValueNameDisplayable();
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

bool CRegListCtrl::deleteItem(const RegItem &item)
{
    bool bSuccess = false;
    NtRegistry ntreg;
    std::wstring subKey(item.getSubKey());
    if(ntreg.create(item.getKey(), subKey.c_str(), subKey.size(), 0, KEY_ALL_ACCESS))
    {
        std::wstring valueName(item.getValueName());
        bSuccess = ntreg.deleteValue(valueName.c_str(), valueName.size());
        if(!bSuccess) {
            std::wstring message;
            message += std::wstring(L"Value:[") + item.getValueNameDisplayable() + L"]";
            MessageBox(message.c_str(), L"Error Delete Value:", MB_ICONERROR);
        }
        ntreg.close();
    } else {
        std::wstring message;
        message += std::wstring(L"SubKey:[") + subKey.c_str() + L"]";
        MessageBox(message.c_str(), L"Error Create Key:", MB_ICONERROR);
    }
    return bSuccess;
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

bool CRegListCtrl::getSelectedItemParamIndex(size_t& index)
{
    bool bSuccess = false;
    if(GetSelectedCount() > 0)
    {
        int nSelected = GetNextItem(-1, LVNI_SELECTED);
        if(nSelected != -1)
        {
            VEC_REGITEM::size_type itemIndex = GetItemData(nSelected);
            if(itemIndex >= 0 && itemIndex < m_keys.size())
            {
                index = itemIndex;
                bSuccess = true;
            }
        }
    }

    return bSuccess;
}

bool CRegListCtrl::getSelectedItemParam(RegItem &item)
{
    bool bSuccess = false;
    size_t index = 0;
    if(getSelectedItemParamIndex(index)) {
        item = m_keys[index];
        bSuccess = true;
    }
    return bSuccess;
}

int CALLBACK CRegListCtrl::CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CRegListCtrl *pListCtrl = (CRegListCtrl *)lParamSort;

    int nCompare = 0;
    std::wstring strItem1 = pListCtrl->m_keys[lParam1].getValueNameDisplayable();
    std::wstring strItem2 = pListCtrl->m_keys[lParam2].getValueNameDisplayable();

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

void CRegListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    size_t itemIndex = 0;
    if(!getSelectedItemParamIndex(itemIndex)) {
        return;
    }

    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    CPoint point(pNMItemActivate->ptAction);
    ::ClientToScreen(pNMHDR->hwndFrom, &point);

    CMenu menu;
    menu.LoadMenu(IDR_LISTVIEWMENU);
    CMenu* pop;
    pop = menu.GetSubMenu(0);

    UINT uCmd = pop->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN |	TPM_RIGHTBUTTON, point.x, point.y, this, NULL );    

    /* Menu item chosen ? */
    if (uCmd != 0)
    {
        switch(uCmd)
        {
        case ID_LISTVIEWMENU_DELETE:
            {
                RegItem item = m_keys[itemIndex];
                if(deleteItem(item)) {
                    m_keys.erase(m_keys.begin() + itemIndex);
                    refresh();
                }
            }
            break;

        case ID_LISTVIEWMENU_COPYPATH:
            {
                RegItem item;
                if(getSelectedItemParam(item)) {
                    std::wstring sPath = item.toString();
                    if(sPath.empty())
                        sPath = Utils::getComputerName();

                    CWnd *pWnd = GetParentOwner();

                    CClipBoard cb;
                    cb.Clear(*pWnd);
                    cb.CopyTo(*pWnd, sPath);
                }
            }
            break;

        case ID_LISTVIEWMENU_REFRESH:
            {
                refresh();
            }
            break;
        }
    }

    *pResult = 0;
}
