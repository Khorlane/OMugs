/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   ChildFrm.cpp                                     *
* Usage:  Not being used                                   *
* Author: Steve Bryant                                     *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "OMugs.h"

/***********************************************************
* Implement dynamic                                        *
************************************************************/

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

/***********************************************************
* CChildFrame message map                                  *
************************************************************/

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/***********************************************************
* CChildFrame constructor                                  *
************************************************************/

CChildFrame::CChildFrame()
{	
}

/***********************************************************
* CChildFrame destructor                                   *
************************************************************/

CChildFrame::~CChildFrame()
{
}

/***********************************************************
* CChildFrame PreCreateWindow                              *
************************************************************/

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// You can modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if ( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}