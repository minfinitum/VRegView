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
    bool getSelectedItemParamIndex(size_t& index);

protected:
    void addItem(const RegItem &item, const unsigned int nSize);
    bool deleteItem(const RegItem &item);

    void refresh();

    static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

private:
    CImageList m_imageList;

protected:
    DECLARE_MESSAGE_MAP()

    RegItem m_current;
    VEC_REGITEM m_keys;
public:
    afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
};


