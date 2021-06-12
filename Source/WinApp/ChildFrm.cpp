/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   ChildFrm.cpp                                     *
* Usage:  Not being used                                   *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
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
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}