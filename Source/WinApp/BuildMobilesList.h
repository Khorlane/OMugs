/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildMobilesList.h                               *
* Usage:  Define BuildMobilesList class                    *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef BUILD_MOBILES_LIST_H
#define BUILD_MOBILES_LIST_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include <afxwin.h>    // MFC core and standard components
#include <afxext.h>    // MFC extensions
#include <afxcmn.h>    // MFC support for common controls
#include <string>      // For std::string
#include <fstream>     // For file operations
#include <tchar.h>     // For _T macro and TCHAR types
#include <filesystem>  // For std::filesystem::directory_iterator
#include "Config.h"
#include "OMugs.h"
#include "BuildMobiles.h"

/***********************************************************
* Define BuildMobilesList class                            *
************************************************************/

class BuildMobilesList : public CDialog
{

// Friend classes
  friend class BuildMobiles;

// Construction
  public:
	  BuildMobilesList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  public:
	  //{{AFX_DATA(BuildMobilesList)
	  enum { IDD = IDD_BuildMobilesList };
	  CListBox	MobilesListBox;
	  //}}AFX_DATA

// Overrides
  public:
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(BuildMobilesList)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

// Implementation
  protected:
	  // Generated message map functions
	  //{{AFX_MSG(BuildMobilesList)
	  virtual BOOL OnInitDialog();
	  virtual void OnCancel();
	  afx_msg void OnClose();
	  virtual void OnOK();
	  afx_msg void OnSelchangeMobilesList();
	  afx_msg void OnRefresh();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
        
// Private functions
  private:
    void            PopulateList();

// Private variables
    CString         MobileId;
};

inline BuildMobilesList *pBuildMobilesList;

#endif