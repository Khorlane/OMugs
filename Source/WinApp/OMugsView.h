/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugsView.h                                      *
* Usage:  Define COMugsView class                          *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef OMUGS_VIEW_H
#define OMUGS_VIEW_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <afxwin.h>    // For MFC core components like CView, CDC
#include <afxext.h>    // For extended MFC components
#include <afxcmn.h>    // For common controls
#include <afxdlgs.h>   // For MFC dialog classes
#include <tchar.h>     // For _T macro and TCHAR types
#include "OMugsDoc.h"  // For COMugsDoc class

/***********************************************************
* Define COMugsView class                                  *
************************************************************/

class COMugsView : public CView
{
protected: // create from serialization only
	COMugsView();
	DECLARE_DYNCREATE(COMugsView)

// Attributes
public:
	COMugsDoc *GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COMugsView)
	public:
	virtual void OnDraw(CDC *pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
	virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
	virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COMugsView();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COMugsView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline COMugsDoc *COMugsView::GetDocument()
   { return (COMugsDoc*)m_pDocument; }

#endif