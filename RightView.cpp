// RightView.cpp : implementation of the CVRegViewView class
//

#include "stdafx.h"
#include "VRegView.h"

#include "VRegViewDoc.h"
#include "RightView.h"
#include "regtreectrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRightView

IMPLEMENT_DYNCREATE(CRightView, CFormView)

BEGIN_MESSAGE_MAP(CRightView, CFormView)
    ON_WM_SIZE()
END_MESSAGE_MAP()

// CRightView construction/destruction

CRightView::CRightView()
    : CFormView(CRightView::IDD)
{
    // TODO: add construction code here

}

CRightView::~CRightView()
{
}

void CRightView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_list);
    DDX_Control(pDX, IDC_EDIT, m_edit);
}

BOOL CRightView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CFormView::PreCreateWindow(cs);
}

void CRightView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit();

    m_list.initialise();
    m_edit.initialise();
}


// CRightView diagnostics

#ifdef _DEBUG
void CRightView::AssertValid() const
{
    CFormView::AssertValid();
}

void CRightView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}

CVRegViewDoc* CRightView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVRegViewDoc)));
    return (CVRegViewDoc*)m_pDocument;
}
#endif //_DEBUG


// CRightView message handlers

void CRightView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    if(m_edit.GetSafeHwnd() == NULL || m_list.GetSafeHwnd() == NULL)
        return;

    CRect rectEdit;
    m_edit.GetWindowRect(&rectEdit);
    ScreenToClient(&rectEdit);

    // edit box is constant height
    static int nEditHeight = rectEdit.bottom - rectEdit.top;

    // Resize listview
    CRect rectList;
    m_list.GetWindowRect(&rectList);
    ScreenToClient(&rectList);

    m_list.MoveWindow(rectList.left, rectList.top, cx, cy - nEditHeight);

    // Resize editbox
    m_edit.GetWindowRect(&rectEdit);
    ScreenToClient(&rectEdit);

    m_edit.MoveWindow(rectEdit.left, cy - nEditHeight, cx, nEditHeight);
}

void CRightView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if(pSender == NULL || pHint == NULL)
        return;

    if(pHint->IsKindOf(RUNTIME_CLASS(CRegTreeCtrl)))
    {
        CRegTreeCtrl *pTree = (CRegTreeCtrl *)pHint;

        RegItem item;
        pTree->getSelectedPath(item);

        m_list.setValueNames(item);
    }
}

BOOL CRightView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if(wParam == IDC_LIST)
    {
        LPNMHDR pNHMDR = (LPNMHDR) lParam;

        if(pNHMDR->code == LVN_ITEMCHANGED)
        {
            RegItem regSelected;
            m_list.getSelectedItemParam(regSelected);

            RegItem item;
            m_edit.setRegValue(regSelected);
        }
    }

    return CFormView::OnNotify(wParam, lParam, pResult);
}
