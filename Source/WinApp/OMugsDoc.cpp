/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugsDoc.cpp                                     *
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

IMPLEMENT_DYNCREATE(COMugsDoc, CDocument)

/***********************************************************
* COMugsDoc message map                                    *
************************************************************/

BEGIN_MESSAGE_MAP(COMugsDoc, CDocument)
	//{{AFX_MSG_MAP(COMugsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/***********************************************************
* COMugsDoc construction                                   *
************************************************************/

COMugsDoc::COMugsDoc()
{
}

/***********************************************************
* COMugsDoc destruction                                    *
************************************************************/

COMugsDoc::~COMugsDoc()
{
}

/***********************************************************
* OnNewDocument                                            *
************************************************************/

BOOL COMugsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
  {
		return FALSE;
  }
	return TRUE;
}

/***********************************************************
* Serialize - not sure what that means??                   *
************************************************************/

void COMugsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}