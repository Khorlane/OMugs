/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugsDoc.h                                       *
* Usage:  Define COMugsDoc class                           *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef OMUGS_DOC_H
#define OMUGS_DOC_H

/***********************************************************
* Define COMugsDoc class                                   *
************************************************************/

class COMugsDoc : public CDocument
{
protected: // create from serialization only
	COMugsDoc();
	DECLARE_DYNCREATE(COMugsDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COMugsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COMugsDoc();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COMugsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif