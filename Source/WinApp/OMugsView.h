/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugsView.h                                      *
* Usage:  Define COMugsView class                          *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef OMUGS_VIEW_H
#define OMUGS_VIEW_H

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