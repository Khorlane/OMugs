/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   HolyQuest.cpp                                    *
* Usage:  The HolyQuest application                        *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#include "stdafx.h"
#include "HolyQuest.h"
#include "HolyQuestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHolyQuestApp

BEGIN_MESSAGE_MAP(CHolyQuestApp, CWinApp)
	//{{AFX_MSG_MAP(CHolyQuestApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHolyQuestApp construction

CHolyQuestApp::CHolyQuestApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHolyQuestApp object

CHolyQuestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHolyQuestApp initialization

BOOL CHolyQuestApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CHolyQuestDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}