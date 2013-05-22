// VRegView.h : main header file for the VRegView application
//
#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "windowstatesaver.h"

// CVRegViewApp:
// See VRegView.cpp for the implementation of this class
//

class CVRegViewApp : public CWinApp
{
public:
    CVRegViewApp();
    virtual ~CVRegViewApp();

    void GetStoredValues(WINDOWPLACEMENT& wp);
    void RestoreWindowState();
    void SaveWindowState();


private:
    WindowStateSaver *m_pState;

// Overrides
public:
    virtual BOOL InitInstance();

// Implementation
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CVRegViewApp theApp;