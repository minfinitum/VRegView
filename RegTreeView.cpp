// RegTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "VRegView.h"
#include "RegTreeView.h"


// CRegTreeView

IMPLEMENT_DYNCREATE(CRegTreeView, CView)

CRegTreeView::CRegTreeView()
{

}

CRegTreeView::~CRegTreeView()
{
}

BEGIN_MESSAGE_MAP(CRegTreeView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CRegTreeView drawing

void CRegTreeView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: add draw code here
}


// CRegTreeView diagnostics

#ifdef _DEBUG
void CRegTreeView::AssertValid() const
{
    CView::AssertValid();
}

#ifndef _WIN32_WCE
void CRegTreeView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CRegTreeView message handlers

int CRegTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create the style
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP |
        LVS_REPORT | TVS_HASLINES | TVS_SHOWSELALWAYS;

    // Create the list control.  Don't worry about specifying
    // correct coordinates.  That will be handled in OnSize()
    BOOL bResult = m_regTreeCtrl.Create(dwStyle, CRect(0,0,0,0), this, IDC_LEFTVIEW); 

    return (bResult ? 0 : -1);
}

void CRegTreeView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    if (::IsWindow(m_regTreeCtrl.m_hWnd))
        m_regTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);
}

BOOL CRegTreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if(wParam == IDC_LEFTVIEW)
    {
        // process tree view notifications
        NMHDR *pNHMDR = (NMHDR *)lParam;

        if(pNHMDR->code == TVN_SELCHANGED)
        {
            //((CMainFrame *)AfxGetMainWnd())->UpdateStatusBar(m_fileTreeCtrl.getSelectedFullPath());

            GetDocument()->UpdateAllViews(this, 0, &m_regTreeCtrl);
        }

    }

    return CView::OnNotify(wParam, lParam, pResult);
}
