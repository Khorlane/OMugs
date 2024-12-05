/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildObjects.h                                   *
* Usage:  Define BuildObjects class                        *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef BUILD_OBJECTS_H
#define BUILD_OBJECTS_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "OMugs.h"
#include "BuildObjectsList.h"

/***********************************************************
* Define BuildObjects class                                *
************************************************************/

class BuildObjects : public CDialog
{

// Friend classes
  friend class BuildObjectsList;

// Construction
  public:
	  BuildObjects(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  public:
	  //{{AFX_DATA(BuildObjects)
	enum { IDD = IDD_BuildObjects };
	CString	Cost;
	CString	Desc1;
	CString	Desc2;
	CString	Desc3;
	CString	DrinkPct;
	CString	FoodPct;
	CString	Names;
	CString	ObjectId;
	CString	Weight;
	CString	WeaponDamage;
	CString	ArmorValue;
	int		  ArmorWearDropList;
	int		  TypeDropList;
	int	  	WeaponTypeDropList;
	//}}AFX_DATA

// Overrides
  public:
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(BuildObjects)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

// Implementation
  protected:
	  // Generated message map functions
	  //{{AFX_MSG(BuildObjects)
	  virtual void OnCancel();
	  afx_msg void OnClose();
	  afx_msg void OnDelete();
	  afx_msg void OnLookup();
	  afx_msg void OnOk();
	  afx_msg void OnSave();
	//}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

// Private functions
  private:
    void            GetSelectedObj();
    void            ParseStuff();
    void            ReadLine();
    void            WriteStuff();

// Private variables
  private:
    int             AfxMsgBox;
    string          ArmorWear;
    fstream         ObjectFile;
    string          ObjectFileName;
    string          Stuff;
    string          Type;
    string          WeaponType;
};

inline BuildObjects *pBuildObjects;

#endif