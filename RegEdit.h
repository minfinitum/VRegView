#pragma once

#include "regitem.h"

#include <string>
#include <vector>

// CRegEdit

class CRegEdit : public CEdit
{
	DECLARE_DYNAMIC(CRegEdit)

public:
	CRegEdit();
	virtual ~CRegEdit();

	void initialise();

	void setRegValue(RegItem &item);

	void setText(const std::wstring &sValue);
	void setMultiString(const std::vector<std::wstring> &vecMultiString);
	void setDWord(DWORD nValue);
	void setBinary(unsigned char *pData, int nSize);

	void clear();

private:
	CFont m_font;

protected:
	DECLARE_MESSAGE_MAP()
};


