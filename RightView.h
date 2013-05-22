// VRegViewView.h : interface of the CVRegViewView class
//


#pragma once

#include "afxcmn.h"
#include "afxwin.h"


#include "regedit.h"
#include "reglistctrl.h"


class CRightView : public CFormView
{
protected: // create from serialization only
	CRightView();
	DECLARE_DYNCREATE(CRightView)

public:
	enum{ IDD = IDD_VREGVIEW_FORM };

// Attributes
public:
	CVRegViewDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CRightView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	CRegListCtrl m_list;
	CRegEdit m_edit;

protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

#ifndef _DEBUG  // debug version in VRegViewView.cpp
inline CVRegViewDoc* CRightView::GetDocument() const
   { return reinterpret_cast<CVRegViewDoc*>(m_pDocument); }
#endif

