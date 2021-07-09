/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildMobilesList.cpp                             *
* Usage:  List dialog for mobiles                          *
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
#include "BuildMobilesList.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* BuildMobilesList constructor                             *
************************************************************/

BuildMobilesList::BuildMobilesList(CWnd *pParent /*=NULL*/)
	: CDialog(BuildMobilesList::IDD, pParent)
{
	//{{AFX_DATA_INIT(BuildMobilesList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  
  // Create modeless dialog box window
  if (Create(BuildMobilesList::IDD, pParent))
  {
    ShowWindow(SW_SHOW);
  }
}

/***********************************************************
* BuildMobilesList dialog data exchanger                   *
************************************************************/

void BuildMobilesList::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BuildMobilesList)
	DDX_Control(pDX, IDC_MobilesList, MobilesListBox);
	//}}AFX_DATA_MAP
}

/***********************************************************
* BuildMobilesList dialog message map                      *
************************************************************/

BEGIN_MESSAGE_MAP(BuildMobilesList, CDialog)
	//{{AFX_MSG_MAP(BuildMobilesList)
	ON_LBN_SELCHANGE(IDC_MobilesList, OnSelchangeMobilesList)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/***********************************************************
* OnInitDialog                                             *
************************************************************/

BOOL BuildMobilesList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	PopulateList();
	return TRUE;
}

/***********************************************************
* OnCancel - delete dialog                                 *
************************************************************/

void BuildMobilesList::OnCancel() 
{	
	CDialog::OnClose();
  delete this;
  pBuildMobilesList = NULL;
}

/***********************************************************
* OnClose - delete dialog                                  *
************************************************************/

void BuildMobilesList::OnClose() 
{
	CDialog::OnClose();
  delete this;
  pBuildMobilesList = NULL;
}

/***********************************************************
* OnOk - delete dialog                                     *
************************************************************/

void BuildMobilesList::OnOK() 
{	
	CDialog::OnClose();
  delete this;
  pBuildMobilesList = NULL;
}

/***********************************************************
* OnRefresh                                                *
************************************************************/

void BuildMobilesList::OnRefresh() 
{
  MobilesListBox.ResetContent();
	PopulateList();
}

/***********************************************************
* OnSelchange MobileList                                   *
************************************************************/

void BuildMobilesList::OnSelchangeMobilesList() 
{
  int     SelectedMob;

  SelectedMob = MobilesListBox.GetCurSel();
  MobilesListBox.GetText(SelectedMob, MobileId);
  if (!pBuildMobiles)
  { // No BuildMobiles dialog, so create and show it
    pBuildMobiles = new BuildMobiles(pCMainFrame);
  }
  pBuildMobiles->GetSelectedMob();
}

/***********************************************************
* Populate MobilesListBox                                  *
************************************************************/

void BuildMobilesList::PopulateList()
{
  CString MobFileName;
  CString MobileId;
  BOOL    MoreFiles;
  CFileFind   FileList;

  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("BuildMobileList::PopulateList - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }

  if (ChgDir(MOBILES_DIR))
  { // Change directory failed
    AfxMessageBox("BuildMobileList::PopulateList - Change directory to MOB_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get a list of all Mobile files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change to home directory failed
    AfxMessageBox("BuildMobileList::PopulateList - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  //***************************
  //* Add mobiles to list box *
  //***************************
  while (MoreFiles)
  { // Process all rooms that have mobiles in them
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    MobFileName = FileList.GetFileName();
    MobileId    = MobFileName.Left(MobFileName.GetLength()-4);
    MobilesListBox.AddString(MobileId);
  }
}