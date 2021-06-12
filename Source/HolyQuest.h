/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   HolyQuest.h                                      *
* Usage:  Main header for HolyQuest application            *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#if !defined(AFX_HOLYQUEST_H__A79E5223_25FE_11D6_8E4B_00B0D052AAF2__INCLUDED_)
#define AFX_HOLYQUEST_H__A79E5223_25FE_11D6_8E4B_00B0D052AAF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHolyQuestApp:
// See HolyQuest.cpp for the implementation of this class
//

class CHolyQuestApp : public CWinApp
{
public:
	CHolyQuestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHolyQuestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHolyQuestApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOLYQUEST_H__A79E5223_25FE_11D6_8E4B_00B0D052AAF2__INCLUDED_)