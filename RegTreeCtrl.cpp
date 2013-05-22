// CRegTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include <vector>
#include <string>

#include "regtreectrl.h"
#include "ntregistry.h"
#include "resource.h"
#include "clipboard.h"
#include "utils.h"

// CRegTreeCtrl

#define			EMPTYITEM		L"emptyitem"

IMPLEMENT_DYNAMIC(CRegTreeCtrl, CTreeCtrl)
CRegTreeCtrl::CRegTreeCtrl()
{
    m_hHKCR = NULL;
    m_hHKCU = NULL;
    m_hHKLM = NULL;
    m_hHU = NULL;
    m_hHKCC = NULL;

}

CRegTreeCtrl::~CRegTreeCtrl()
{
    m_imageList.Detach();
}


BEGIN_MESSAGE_MAP(CRegTreeCtrl, CTreeCtrl)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CRegTreeCtrl::OnTvnItemexpanding)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CRegTreeCtrl::OnNMCustomdraw)
    ON_WM_CONTEXTMENU()
    ON_NOTIFY_REFLECT(NM_RCLICK, &CRegTreeCtrl::OnNMRclick)
END_MESSAGE_MAP()


// CRegTreeCtrl message handlers

void CRegTreeCtrl::initialise()
{
    loadImageList();
    populate();
}

void CRegTreeCtrl::loadImageList()
{
    // Initialise image list
    m_imageList.Create(16, 14, ILC_COLOR8 | ILC_MASK, 0, 2);

    CBitmap bmFolderClosed;
    bmFolderClosed.LoadBitmap(IDB_FOLDER_CLOSED);
    m_imageList.Add(&bmFolderClosed, RGB(255, 0, 255));

    CBitmap bmFolderOpen;
    bmFolderOpen.LoadBitmap(IDB_FOLDER_OPEN);
    m_imageList.Add(&bmFolderOpen, RGB(255, 0, 255));

    CBitmap bmComputer;
    bmComputer.LoadBitmap(IDB_COMPUTER);
    m_imageList.Add(&bmComputer, RGB(255, 0, 255));

    SetImageList(&m_imageList, LVSIL_NORMAL);
}

void CRegTreeCtrl::populate()
{
    m_hRoot = InsertItem(Utils::getComputerName().c_str(), TVI_ROOT, TVI_LAST);
    SetItemImage(m_hRoot, 2, 2);

    m_hHKCR = InsertItem(L"HKEY_CLASSES_ROOT", m_hRoot, TVI_LAST);
    InsertItem(EMPTYITEM, m_hHKCR, TVI_LAST);

    m_hHKCU = InsertItem(L"HKEY_CURRENT_USER", m_hRoot, TVI_LAST);
    InsertItem(EMPTYITEM, m_hHKCU, TVI_LAST);

    m_hHKLM = InsertItem(L"HKEY_LOCAL_MACHINE", m_hRoot, TVI_LAST);
    InsertItem(EMPTYITEM, m_hHKLM, TVI_LAST);

    m_hHU = InsertItem(L"HKEY_USERS", m_hRoot, TVI_LAST);
    InsertItem(EMPTYITEM, m_hHU, TVI_LAST);

    m_hHKCC = InsertItem(L"HKEY_CURRENT_CONFIG", m_hRoot, TVI_LAST);
    InsertItem(EMPTYITEM, m_hHKCC, TVI_LAST);

    Expand(m_hRoot, TVE_EXPAND);
}

bool CRegTreeCtrl::getPath(HTREEITEM hItem, RegItem &regitem)
{
    HTREEITEM hCurrent = hItem;
    CString sItem;

    HKEY hKey = NULL;
    std::wstring sSubKey;

    std::wstring sCurrentPath;
    bool bContinue = true;
    while(bContinue && hCurrent != NULL)
    {
        if(hCurrent == m_hRoot)
        {
            bContinue = false;
        }
        else if(hCurrent == m_hHKCR)
        {
            hKey = HKEY_CLASSES_ROOT;
            bContinue = false;
        }
        else if(hCurrent == m_hHKCU)
        {
            hKey = HKEY_CURRENT_USER;
            bContinue = false;
        }
        else if(hCurrent == m_hHKLM)
        {
            hKey = HKEY_LOCAL_MACHINE;
            bContinue = false;
        }
        else if(hCurrent == m_hHU)
        {
            hKey = HKEY_USERS;
            bContinue = false;
        }
        else if(hCurrent == m_hHKCC)
        {
            hKey = HKEY_CURRENT_CONFIG;
            bContinue = false;
        }
        else
        {
            sItem = GetItemText(hCurrent);

            sCurrentPath = sSubKey; 
            sSubKey = sItem.GetBuffer();

            if(!sCurrentPath.empty())
                sSubKey += L"\\" + sCurrentPath;
        }

        hCurrent = GetParentItem(hCurrent);
    }

    regitem.set(hKey, sSubKey, L"");

    return true;
}

void CRegTreeCtrl::expandingItem(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    UINT uiAction = pNMTreeView->action;

    if(uiAction == TVE_EXPAND)
    {
        HTREEITEM hChildItem = GetChildItem(hItem);
        CString sItemText = GetItemText(hChildItem);
        if(sItemText.CompareNoCase(EMPTYITEM) == 0)
        {
            LoadItem(hItem);
        }
    }
    else if(uiAction == TVE_COLLAPSE)
    {
        if(hItem != m_hRoot)
            SetItemImage(hItem, 0, 0);
    }

    *pResult = 0;
}

void CRegTreeCtrl::LoadItem(HTREEITEM hItem)
{
    if(hItem != m_hRoot)
        SetItemImage(hItem, 1, 1);

    // Delete all of the children of hItem.
    if (ItemHasChildren(hItem))
    {
       HTREEITEM hNextItem;
       HTREEITEM hChildItem = GetChildItem(hItem);
       while (hChildItem != NULL)
       {
          hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
          DeleteItem(hChildItem);
          hChildItem = hNextItem;
       }
    }

    // Prepare for update
    SetRedraw(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    HTREEITEM hParent = GetParentItem(hItem);

    NtRegistry ntreg;
    std::vector<std::wstring> subkeys;
    std::vector<std::wstring>::iterator iterSubKey;

    RegItem regitem;
    if(getPath(hItem, regitem))
    {
        if(ntreg.open(regitem.getKey(), regitem.getSubKey().c_str()))
        {
            ntreg.listSubKeys(subkeys);
            ntreg.close();
        }

        std::wstring sSubKeyPath;
        HTREEITEM hNewItem = NULL;
        BOOL bIsHidden = FALSE;

        for(iterSubKey = subkeys.begin(); iterSubKey != subkeys.end(); ++iterSubKey)
        {
            hNewItem = InsertItem(iterSubKey->c_str(), hItem, TVI_LAST);

            sSubKeyPath = regitem.getSubKey() + L"\\" + *iterSubKey;
            bIsHidden = FALSE;

            if(ntreg.open(regitem.getKey(), sSubKeyPath.c_str(), bIsHidden))
            {
                if(ntreg.getSubKeysCount() > 0)
                {
                    InsertItem(EMPTYITEM, hNewItem, TVI_LAST);
                }

                // Set item state to bold if a hidden key is found.
                SetItemState(hNewItem, bIsHidden ? TVIS_BOLD : 0, TVIS_BOLD);
                SetItemData(hNewItem, bIsHidden);
            }
            ntreg.close();
        }

        SortChildren(hItem);
        Expand(hItem, TVE_EXPAND);
    }

    // Update complete
    SetCursor(LoadCursor (NULL, IDC_ARROW));
    SetRedraw(TRUE);
}

bool CRegTreeCtrl::getSelectedPath(RegItem &regitem)
{
    bool bSuccess = false;
    HTREEITEM hItem = GetSelectedItem();
    std::wstring sPath;
    if(hItem != NULL)
        bSuccess = getPath(hItem, regitem);
    return bSuccess;
}

std::wstring CRegTreeCtrl::getSelectedPath()
{
    HTREEITEM hItem = GetSelectedItem();
    std::wstring sPath;
    if(hItem != NULL)
    {
        RegItem regitem;
        if(getPath(hItem, regitem))
        {
            sPath = regitem.toString();

            if(sPath.empty())
                sPath = Utils::getComputerName();
        }
    }
    return sPath;
}

void CRegTreeCtrl::onRefresh()
{
    HTREEITEM hItem = GetSelectedItem();
    LoadItem(hItem);
}

void CRegTreeCtrl::onCopyPathToClipboard()
{
    std::wstring sPath = getSelectedPath();
    CWnd *pWnd = GetParentOwner();

    CClipBoard cb;
    cb.Clear(*pWnd);
    cb.CopyTo(*pWnd, sPath);
}

void CRegTreeCtrl::onDeleteSubKey()
{
    RegItem regitem;
    if(getSelectedPath(regitem)) {
        NtRegistry ntreg;
        if(ntreg.create(regitem.getKey(), regitem.getSubKey().c_str(), 0, KEY_ALL_ACCESS))
        {
            ntreg.deleteSubKey();
            ntreg.close();
        }
    }

    HTREEITEM hItem = GetSelectedItem();
    HTREEITEM hParent = GetParentItem(hItem);
    SelectItem(hParent);
    onRefresh();
}

void CRegTreeCtrl::OnNMCustomdrawTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW* pCD = (NMLVCUSTOMDRAW*)pNMHDR;

    switch (pCD->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT:

        if(pCD->nmcd.lItemlParam)
            pCD->clrText = RGB(255, 0, 0);

        break;

    default:// it wasn't a notification that was interesting to us.
        *pResult = CDRF_DODEFAULT;
        break;
    }

}

void CRegTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
    expandingItem(pNMHDR, pResult);

    *pResult = 0;
}

void CRegTreeCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    OnNMCustomdrawTree(pNMHDR, pResult);

    *pResult = 0;
}

void CRegTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
    if (point.x == -1)
    {
        HTREEITEM ht = GetSelectedItem();
        RECT rect;

        GetItemRect( ht, &rect, true );
        ClientToScreen( &rect );

        /* Offset the popup menu origin so
        * we can read some of the text
        */

        point.x = rect.left + 15;
        point.y = rect.top + 8;
    }

    CMenu menu;
    menu.LoadMenu(IDR_TREEVIEWMENU);

    CMenu * pop;
    pop = menu.GetSubMenu(0);

    UINT uCmd = pop->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN |	TPM_RIGHTBUTTON, point.x, point.y, this, NULL );

    /* Menu item chosen ? */
    if (uCmd != 0)
    {
        switch(uCmd)
        {
        case ID_TREEVIEWMENU_REFRESH:
            onRefresh();
            break;

        case ID_TREEVIEWMENU_COPYPATH:
            onCopyPathToClipboard();
            break;

        case ID_TREEVIEWMENU_DELETE:
            onDeleteSubKey();
            break;
        }
    }

    menu.DestroyMenu();
}

void CRegTreeCtrl::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
    /* Get the mouse cursor position */
    DWORD dwPos = GetMessagePos();

    /* Convert the co-ords into a CPoint structure */
    CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
    CPoint spt = pt;

    /* Convert to screen co-ords for hittesting */
    ScreenToClient(&spt);

    UINT test;
    HTREEITEM hti = HitTest( spt, &test );

    /* Did the click occur on an item */
    if((hti != NULL) && (test & TVHT_ONITEM))
    {
        HTREEITEM htCur = GetSelectedItem();

        /* Do the context menu */
        OnContextMenu(this, pt);
    }

    *pResult = 0;
}

