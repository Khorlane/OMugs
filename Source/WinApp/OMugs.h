/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugs.h                                          *
* Usage:  Define COMugsApp class                           *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef OMUGS_H
#define OMUGS_H

/***********************************************************
* Includes                                                 *
************************************************************/

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "OMugsDoc.h"
#include "OMugsView.h"
#include "Utility.h"

/***********************************************************
* Define COMugsApp                                         *
************************************************************/

class COMugsApp : public CWinApp
{
public:
	COMugsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COMugsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(COMugsApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
