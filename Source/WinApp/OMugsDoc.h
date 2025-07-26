/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugsDoc.h                                       *
* Usage:  Define COMugsDoc class                           *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef OMUGS_DOC_H
#define OMUGS_DOC_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <afxwin.h>    // For MFC core components like CDocument
#include <afxext.h>    // For extended MFC components
#include <afxcmn.h>    // For common controls
#include <tchar.h>     // For _T macro and TCHAR types
#include <string>      // For std::string
#include <afx.h>       // For CArchive and serialization support

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