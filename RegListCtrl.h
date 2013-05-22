#pragma once

#include <string>
#include <vector>

#include "regitem.h"

// CRegListCtrl

class CRegListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CRegListCtrl)

public:
	CRegListCtrl();
	virtual ~CRegListCtrl();

	void initialise();
	void setValueNames(const RegItem &item);

	bool getSelectedItem(std::wstring &sValueName);
	bool getSelectedItemParam(RegItem &item);

	void addKeysFromPath(std::wstring &sPath);

protected:
	void addItem(const RegItem &item, const unsigned int nSize);
	static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
	CImageList m_imageList;

protected:
	DECLARE_MESSAGE_MAP()

	VEC_REGITEM m_keys;
};


