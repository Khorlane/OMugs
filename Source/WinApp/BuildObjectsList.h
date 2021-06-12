/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildObjectsList.h                               *
* Usage:  Define BuildObjectsList class                    *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef BUILD_OBJECTS_LIST_H
#define BUILD_OBJECTS_LIST_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "OMugs.h"
#include "BuildObjects.h"

/***********************************************************
* Define BuildObjectsList class                            *
************************************************************/

class BuildObjectsList : public CDialog
{

// Friend classes
  friend class BuildObjects;

// Construction
  public:
	  BuildObjectsList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
  public:
	  //{{AFX_DATA(BuildObjectsList)
	enum { IDD = IDD_BuildObjectsList };
	  CListBox	ObjectsListBox;
	  int		    TypeDropList;
	  int		    ArmorWearDropList;
	int		WeaponTypeDropList;
	//}}AFX_DATA

// Overrides
  public:
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(BuildObjectsList)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

// Implementation
  protected:
	  // Generated message map functions
	  //{{AFX_MSG(BuildObjectsList)
	  virtual BOOL OnInitDialog();
	  virtual void OnCancel();
	  afx_msg void OnClose();
	  virtual void OnOK();
	  afx_msg void OnSelchangeObjectsList();
	  afx_msg void OnRefresh();
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
        
// Private functions
  private:
    void            PopulateList();
    bool            PopulateListOk();
    void            ReadLine();
    void            SetFilter();

// Private variables
    CString         ArmorWear;
    CFileFind       FileList;
    BOOL            MoreFiles;
    CString         ObjectId;
    CStdioFile      ObjectFile;
    CString         ObjectFileName;
    CString         TmpStr;
    CString         Type;
    CString         Stuff;
    int             Success;
    CString         WeaponType;
};

#endif
