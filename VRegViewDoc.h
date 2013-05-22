// VRegViewDoc.h : interface of the CVRegViewDoc class
//


#pragma once


class CVRegViewDoc : public CDocument
{
protected: // create from serialization only
	CVRegViewDoc();
	DECLARE_DYNCREATE(CVRegViewDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CVRegViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


