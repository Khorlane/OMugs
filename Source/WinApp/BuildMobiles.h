/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildMobiles.h                                   *
* Usage:  Define BuildMobiles class                        *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef BUILD_MOBILES_H
#define BUILD_MOBILES_H

/***********************************************************
* Includes                                                 *
************************************************************/

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
	  CString	Talk;
	  CString	Level;
	  CString	Loot;
	  CString	HitPoints;
	  CString	ExpPoints;
	  CString	Desc2;
	  CString	Desc1;
	  CString	Damage;
	  CString	Armor;
	  BOOL	ActionAggro;
	  BOOL	ActionDestroy;
	  BOOL	ActionFaction;
	  BOOL	ActionHelp;
	  BOOL	ActionNoMove;
	  BOOL	ActionNone;
	  BOOL	ActionWimpy;
	  CString	Desc3;
	  int		FactionRadio;
	  int		SexRadio;
	  int		AttackRadio;
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
    CString         Action;
    int             AfxMsgBox;
    CString         Attack;
    CString         Faction;
    CStdioFile      MobileFile;
    CString         MobileFileName;
    CString         Sex;
    CString         Stuff;
    int             Success;
};

inline BuildMobiles *pBuildMobiles;

#endif