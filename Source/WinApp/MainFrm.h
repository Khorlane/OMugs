/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   MainFrm.h                                        *
* Usage:  Define CMainFrame class                          *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "OMugs.h"
#include "BuildMobiles.h"
#include "BuildMobilesList.h"
#include "BuildObjects.h"
#include "BuildObjectsList.h"
#include "GenerateRooms.h"
#include "LineCount.h"

/***********************************************************
* Define CMainFrame class                                  *
************************************************************/

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

	static UINT StartBigDog (LPVOID param);
	
	typedef struct THREADSTRUCT
	{
		CMainFrame*	_this;
	} THREADSTRUCT;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnControlStartGame();
	afx_msg void OnUpdateControlStartGame(CCmdUI* pCmdUI);
	afx_msg void OnBuildMobiles();
	afx_msg void OnControlStopGame();
	afx_msg void OnUpdateControlStopGame(CCmdUI* pCmdUI);
	afx_msg void OnGenerateRooms();
	afx_msg void OnGenerateLinecount();
	afx_msg void OnBuildObjects();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  int AfxMsgBox;

};

#endif