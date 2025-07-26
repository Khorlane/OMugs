/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugs.h                                          *
* Usage:  Define COMugsApp class                           *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef OMUGS_H
#define OMUGS_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <afxwin.h>    // For MFC core components like CWinApp
#include <afxext.h>    // For extended MFC components
#include <afxcmn.h>    // For common controls
#include <afxdlgs.h>   // For MFC dialog classes
#include <afxsock.h>   // For MFC socket classes
#include <tchar.h>     // For _T macro and TCHAR types
#include <string>      // For std::string
#include "resource.h"  // For resource definitions
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

inline CWnd  *pCMainFrame;
inline void		BigDog();

#endif
