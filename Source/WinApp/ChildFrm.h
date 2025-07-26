/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   ChildFrm.h                                       *
* Usage:  Define CChildFrame class                         *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef CHILD_FRAME_H
#define CHILD_FRAME_H

#include <afxwin.h>    // For MFC core components like CMDIChildWnd
#include <afxext.h>    // For extended MFC components
#include <afxcmn.h>    // For common controls
#include <afxdlgs.h>   // For MFC dialog classes
#include <tchar.h>     // For _T macro and TCHAR types

/***********************************************************
* Define CChildFrame class                                 *
************************************************************/

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif