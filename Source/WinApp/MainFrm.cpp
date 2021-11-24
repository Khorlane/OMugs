/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   MainFrm.cpp                                      *
* Usage:  Main window for OMugs application                *
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
* Globals                                                  *
************************************************************/

bool              MenuControlStartGameEnabled = true;
bool              MenuControlStopGameEnabled  = false;

/***********************************************************
* Implement dynamic                                        *
************************************************************/

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

/***********************************************************
* CMainFrame message map                                   *
************************************************************/

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_CONTROL_START_GAME, OnControlStartGame)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_START_GAME, OnUpdateControlStartGame)
	ON_COMMAND(ID_BUILD_MOBILES, OnBuildMobiles)
	ON_COMMAND(ID_CONTROL_STOP_GAME, OnControlStopGame)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_STOP_GAME, OnUpdateControlStopGame)
	ON_COMMAND(ID_GENERATE_ROOMS, OnGenerateRooms)
	ON_COMMAND(ID_GENERATE_LINECOUNT, OnGenerateLinecount)
	ON_COMMAND(ID_BUILD_OBJECTS, OnBuildObjects)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/***********************************************************
* CMainFrame constructor                                   *
************************************************************/

CMainFrame::CMainFrame()
{
}

/***********************************************************
* CMainFrame destructor                                    *
************************************************************/

CMainFrame::~CMainFrame()
{
}

/***********************************************************
* CMainFrame OnCreate                                      *
************************************************************/

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

  pCMainFrame = this;

	return 0;
}

/***********************************************************
* CMainFrame PreCreateWindow                               *
************************************************************/

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// You can modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/***********************************************************
* Code to start BigDog as a separate thread                *
************************************************************/

UINT CMainFrame::StartBigDog(LPVOID param)
{
  int AfxMsgBox;
  THREADSTRUCT *ts = (THREADSTRUCT*)param;

  AfxMsgBox = AfxMessageBox("Click OK to START the Online Multi-User Game Server", MB_OKCANCEL);
  if (AfxMsgBox == IDOK)
  {
    MenuControlStartGameEnabled = false;
    MenuControlStopGameEnabled  = true;
    BigDog();
    AfxMessageBox("The Online Multi-User Game Server has Stopped");
    MenuControlStartGameEnabled = true;
    MenuControlStopGameEnabled  = false;
  }
  else
  if (AfxMsgBox == IDCANCEL)
  {
    MenuControlStartGameEnabled = true;
    MenuControlStopGameEnabled  = false;
    AfxMessageBox("You pressed CANCEL, the Online Multi-User Game Server was NOT started");
  }
  AfxEndThread(0);
  return 1;
}

/***********************************************************
* OnControlStartGame - Menu: Control->Start                *
************************************************************/

void CMainFrame::OnControlStartGame() 
{
  THREADSTRUCT *_param = new THREADSTRUCT;
  _param->_this = this;
  AfxBeginThread (StartBigDog, _param);
}

/***********************************************************
* OnUpdateControlStartGame                                 *
************************************************************/

void CMainFrame::OnUpdateControlStartGame(CCmdUI *pCmdUI) 
{
  pCmdUI->Enable(MenuControlStartGameEnabled);
}

/***********************************************************
* OnControlStopGame - Menu: Control->Stop                  *
************************************************************/

void CMainFrame::OnControlStopGame() 
{
  CString GoGoGoFileName;
  CString StopItFileName;

  GoGoGoFileName  = CONTROL_DIR;
  GoGoGoFileName += "GoGoGo";
  StopItFileName  = CONTROL_DIR;
  StopItFileName += "StopIt";
  CFile::Rename(GoGoGoFileName, StopItFileName);
}

/***********************************************************
* OnUpdateControlStopGame                                  *
************************************************************/

void CMainFrame::OnUpdateControlStopGame(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(MenuControlStopGameEnabled);
}

/***********************************************************
* OnBuildMobiles - Menu: Build->Mobiles                    *
************************************************************/

void CMainFrame::OnBuildMobiles() 
{
  if (!pBuildMobiles)
  { // No BuildMobiles dialog, so create and show it
    pBuildMobiles = new BuildMobiles(this);
  }
  if (!pBuildMobilesList)
  { // No BuildMobilesList dialog, so create and show it
    pBuildMobilesList = new BuildMobilesList(this);
  }
}

/***********************************************************
* OnBuildObjects - Menu: Build->Objects                    *
************************************************************/

void CMainFrame::OnBuildObjects() 
{
  if (!pBuildObjects)
  { // No BuildObjects dialog, so create and show it
    pBuildObjects = new BuildObjects(this);
  }
  if (!pBuildObjectsList)
  { // No BuildObjectsList dialog, so create and show it
    pBuildObjectsList = new BuildObjectsList(this);
  }
}

/***********************************************************
* OnGenerateRooms - Menu: Generate->Rooms                  *
************************************************************/

void CMainFrame::OnGenerateRooms() 
{
  GenerateRooms *pGenerateRooms;

  AfxMsgBox = AfxMessageBox("Click OK to generate rooms", MB_OKCANCEL);
  if (AfxMsgBox == IDOK)
  {
    pGenerateRooms = new GenerateRooms;
    delete pGenerateRooms;
    AfxMessageBox("Room generation is complete");
  }
  else
  if (AfxMsgBox == IDCANCEL)
  {
    AfxMessageBox("You pressed CANCEL, no rooms were generated");
  }
}

/***********************************************************
* OnGenerateLineCount - Menu: Generate->Line Count         *
************************************************************/

void CMainFrame::OnGenerateLinecount() 
{
  LineCount *pLineCount;

  AfxMsgBox = AfxMessageBox("Click OK to generate line count report", MB_OKCANCEL);
  if (AfxMsgBox == IDOK)
  {
    pLineCount = new LineCount;
    delete pLineCount;
    AfxMessageBox("Line count report generation is complete");
  }
  else
  if (AfxMsgBox == IDCANCEL)
  {
    AfxMessageBox("You pressed CANCEL, no line count report was generated");
  }
}