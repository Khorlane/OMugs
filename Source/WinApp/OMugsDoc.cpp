/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugsDoc.cpp                                     *
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