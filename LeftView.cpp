// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "VRegView.h"
#include "VRegViewDoc.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CRegTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CRegTreeView)
END_MESSAGE_MAP()


// CLeftView construction/destruction

CLeftView::CLeftView()
{
    // TODO: add construction code here
}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

    return CRegTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
    CRegTreeView::OnInitialUpdate();

    m_regTreeCtrl.initialise();
}


// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
    CRegTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
    CRegTreeView::Dump(dc);
}

CVRegViewDoc* CLeftView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVRegViewDoc)));
    return (CVRegViewDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView message handlers
