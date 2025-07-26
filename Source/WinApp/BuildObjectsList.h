/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildObjectsList.h                               *
* Usage:  Define BuildObjectsList class                    *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef BUILD_OBJECTS_LIST_H
#define BUILD_OBJECTS_LIST_H

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
    string          ArmorWear;
    CString         ObjectId;
    ifstream        ObjectFile;
    string          ObjectFileName;
    string          Type;
    string          WeaponType;
};

inline BuildObjectsList *pBuildObjectsList;

#endif
