/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildMobiles.h                                   *
* Usage:  Define BuildMobiles class                        *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef BUILD_MOBILES_H
#define BUILD_MOBILES_H

/***********************************************************
* Includes                                                 *
************************************************************/
#include <afxwin.h>    // MFC core and standard components
#include <afxext.h>    // MFC extensions
#include <afxcmn.h>    // MFC support for common controls
#include <string>      // For std::string
#include <fstream>     // For file operations
#include <iostream>    // For std::cout
#include <tchar.h>     // For _T macro and TCHAR types

#include "Config.h"
#include "OMugs.h"
#include "BuildMobilesList.h"

/***********************************************************
* Define BuildMobiles class                                *
************************************************************/

class BuildMobiles : public CDialog
{

// Friend classes
  friend class BuildMobilesList;

// Construction
  public:
	  BuildMobiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  public:
	  //{{AFX_DATA(BuildMobiles)
	  enum { IDD = IDD_BuildMobiles };
	  CString	MobileId;
	  CString	Names;
	  CString Talk;
	  CString Level;
	  CString Loot;
	  CString HitPoints;
	  CString ExpPoints;
	  CString Desc2;
	  CString Desc1;
	  CString Damage;
	  CString Armor;
	  BOOL	  ActionAggro;
	  BOOL	  ActionDestroy;
	  BOOL  	ActionFaction;
	  BOOL	  ActionHelp;
	  BOOL  	ActionNoMove;
	  BOOL  	ActionNone;
	  BOOL	  ActionWimpy;
	  CString	Desc3;
	  int	  	FactionRadio;
	  int		  SexRadio;
	  int		  AttackRadio;
	  //}}AFX_DATA

// Overrides
  public:
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(BuildMobiles)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

// Implementation
  protected:
	  // Generated message map functions
	  //{{AFX_MSG(BuildMobiles)
	  afx_msg void OnActionNone();
	  afx_msg void OnActionAggro();
	  afx_msg void OnActionFaction();
	  afx_msg void OnActionDestroy();
	  afx_msg void OnActionHelp();
	  afx_msg void OnActionNoMove();
	  afx_msg void OnActionWimpy();
	  virtual void OnCancel();
	  afx_msg void OnClose();
	  afx_msg void OnDelete();
	  afx_msg void OnLookup();
	  afx_msg void OnOk();
	  afx_msg void OnSave();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP();

// Private functions
  private:
    void            GetSelectedMob();
    void            ParseStuff();
    void            ReadLine();
    void            WriteStuff();

// Private variables
  private:
    string          Action;
    int             AfxMsgBox;
    string          Attack;
    string          Faction;
    fstream         MobileFile;
    string          MobileFileName;
    string          Sex;
};

inline BuildMobiles *pBuildMobiles;

#endif