#pragma once

#include "regtreectrl.h"

// CRegTreeView view

class CRegTreeView : public CView
{
	DECLARE_DYNCREATE(CRegTreeView)

protected:
	CRegTreeView();           // protected constructor used by dynamic creation
	virtual ~CRegTreeView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

	CRegTreeCtrl m_regTreeCtrl;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


