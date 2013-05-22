// VRegViewDoc.cpp : implementation of the CVRegViewDoc class
//

#include "stdafx.h"
#include "VRegView.h"
#include "VRegViewDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVRegViewDoc

IMPLEMENT_DYNCREATE(CVRegViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CVRegViewDoc, CDocument)
END_MESSAGE_MAP()


// CVRegViewDoc construction/destruction

CVRegViewDoc::CVRegViewDoc()
{
    // TODO: add one-time construction code here

}

CVRegViewDoc::~CVRegViewDoc()
{
}

BOOL CVRegViewDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

// CVRegViewDoc serialization

void CVRegViewDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}


// CVRegViewDoc diagnostics

#ifdef _DEBUG
void CVRegViewDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CVRegViewDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CVRegViewDoc commands
