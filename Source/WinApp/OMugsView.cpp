/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   OMugsView.cpp                                    *
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

IMPLEMENT_DYNCREATE(COMugsView, CView)

/***********************************************************
* COMugsView message map                                   *
************************************************************/

BEGIN_MESSAGE_MAP(COMugsView, CView)
	//{{AFX_MSG_MAP(COMugsView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/***********************************************************
* COMugsView construction                                  *
************************************************************/

COMugsView::COMugsView()
{
}

/***********************************************************
* COMugsView destruction                                   *
************************************************************/

COMugsView::~COMugsView()
{
}

/***********************************************************
* COMugsView PreCreateWindow                               *
************************************************************/

BOOL COMugsView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/***********************************************************
* OnDraw message handler                                   *
************************************************************/

void COMugsView::OnDraw(CDC* pDC)
{
	COMugsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/***********************************************************
* OnPreparePrinting message handler                        *
************************************************************/

BOOL COMugsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

/***********************************************************
* OnBeginPrinting message handler                          *
************************************************************/

void COMugsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/***********************************************************
* OnEndPrinting message handler                            *
************************************************************/

void COMugsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}