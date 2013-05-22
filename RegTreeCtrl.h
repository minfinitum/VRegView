#pragma once

#include <string>

#include "regitem.h"

// CRegTreeCtrl

class CRegTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CRegTreeCtrl)

public:
	CRegTreeCtrl();
	virtual ~CRegTreeCtrl();

	void initialise();
	void expandingItem(NMHDR *pNMHDR, LRESULT *pResult);

	bool getSelectedPath(RegItem &regitem);
	std::wstring getSelectedPath();

	void OnNMCustomdrawTree(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	void loadImageList();
    void LoadItem(HTREEITEM hItem);
	void populate();

	void onRefresh();
	void onCopyPathToClipboard();
    void onDeleteSubKey();

	bool getPath(HTREEITEM hItem, RegItem &regItem);

	DECLARE_MESSAGE_MAP()
public:
	CImageList m_imageList;

	HTREEITEM m_hRoot;
	HTREEITEM m_hHKCR;
	HTREEITEM m_hHKCU;
	HTREEITEM m_hHKLM;
	HTREEITEM m_hHU;
	HTREEITEM m_hHKCC;

	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
};


