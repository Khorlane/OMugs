/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildObjectsList.cpp                             *
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

#include "stdafx.h" // This is only here to speed up compiles
#include "BuildObjectsList.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* BuildObjectsList constructor                             *
************************************************************/

BuildObjectsList::BuildObjectsList(CWnd *pParent /*=NULL*/)
	: CDialog(BuildObjectsList::IDD, pParent)
{
	//{{AFX_DATA_INIT(BuildObjectsList)
	TypeDropList = -1;
	ArmorWearDropList = -1;
	WeaponTypeDropList = -1;
	//}}AFX_DATA_INIT
  
  // Create modeless dialog box window
  if (Create(BuildObjectsList::IDD, pParent))
  {
    ShowWindow(SW_SHOW);
  }
}

/***********************************************************
* BuildObjectsList dialog data exchanger                   *
************************************************************/

void BuildObjectsList::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BuildObjectsList)
	DDX_Control(pDX, IDC_ObjectsList, ObjectsListBox);
	DDX_CBIndex(pDX, IDC_Type, TypeDropList);
	DDX_CBIndex(pDX, IDC_ArmorWear, ArmorWearDropList);
	DDX_CBIndex(pDX, IDC_WeaponType, WeaponTypeDropList);
	//}}AFX_DATA_MAP
}

/***********************************************************
* BuildObjectsList dialog message map                      *
************************************************************/

BEGIN_MESSAGE_MAP(BuildObjectsList, CDialog)
	//{{AFX_MSG_MAP(BuildObjectsList)
	ON_LBN_SELCHANGE(IDC_ObjectsList, OnSelchangeObjectsList)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//==========================================================
//= Message handler functions                              =
//==========================================================

/***********************************************************
* OnInitDialog                                             *
************************************************************/

BOOL BuildObjectsList::OnInitDialog() 
{
	CDialog::OnInitDialog();
  SetFilter();
	PopulateList();
	return TRUE;
}

/***********************************************************
* OnCancel - delete dialog                                 *
************************************************************/

void BuildObjectsList::OnCancel() 
{	
	CDialog::OnClose();
  delete this;
  pBuildObjectsList = NULL;
}

/***********************************************************
* OnClose - delete dialog                                  *
************************************************************/

void BuildObjectsList::OnClose() 
{
	CDialog::OnClose();
  delete this;
  pBuildObjectsList = NULL;
}

/***********************************************************
* OnOk - delete dialog                                     *
************************************************************/

void BuildObjectsList::OnOK() 
{	
	CDialog::OnClose();
  delete this;
  pBuildObjectsList = NULL;
}

/***********************************************************
* OnRefresh                                                *
************************************************************/

void BuildObjectsList::OnRefresh() 
{
  ObjectsListBox.ResetContent();
  UpdateData(true); // Get data from dialog
  SetFilter();
	PopulateList();
  // Reset drop down lists
  if (Type == "all")
  { // Type is all, blank out ArmorWear and WeaponType
	  ArmorWearDropList  = -1;
	  WeaponTypeDropList = -1;
  }
  else
  if (Type == "armor")
  { // Type is armor, blank out WeaponType
	  WeaponTypeDropList = -1;
  }
  else
  if (Type == "weapon")
  { // Type is weapon, blank out ArmorWear
	  ArmorWearDropList  = -1;
  }
  UpdateData(false); // Put data to dialog
}

/***********************************************************
* OnSelchange ObjectList                                   *
************************************************************/

void BuildObjectsList::OnSelchangeObjectsList() 
{
  int SelectedObj;

  SelectedObj = ObjectsListBox.GetCurSel();
  ObjectsListBox.GetText(SelectedObj, ObjectId);
  if (!pBuildObjects)
  { // No BuildObjects dialog, so create and show it
    pBuildObjects = new BuildObjects(pCMainFrame);
  }
  pBuildObjects->GetSelectedObj();
}

//==========================================================
//= Message handler support functions                      =
//==========================================================

/***********************************************************
* Populate ObjectsList Box                                 *
************************************************************/

void BuildObjectsList::PopulateList()
{
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("BuildObjectList::PopulateList - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  if (ChgDir(OBJECTS_DIR))
  { // Change directory failed
    AfxMessageBox("BuildObjectList::PopulateList - Change directory to OBJECTS_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get a list of all Object files
  if (ChgDir(HomeDir))
  { // Change to home directory failed
    AfxMessageBox("BuildObjectList::PopulateList - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  //***************************
  //* Add objects to list box *
  //***************************
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    { // Skip directories
      continue;
    }
    ObjectFileName = entry.path().filename().string();
    ObjectId = StrLeft(ObjectFileName, StrGetLength(ObjectFileName)-4).c_str();
    if (PopulateListOk())
    { // Object passed through the filters, add it to the list
      ObjectsListBox.AddString(ObjectId);
    }
  }
}

/***********************************************************
* Populate ObjectsList Box Ok?                             *
************************************************************/

bool BuildObjectsList::PopulateListOk()
{
  if (Type == "all")
  { // No Type filter
    return true;
  }
  ObjectFileName =  OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  ObjectFile.open(ObjectFileName);
  if (!ObjectFile.is_open())
  { // Mobile not found
    AfxMessageBox("BuildObjectsList::PopulateListOk - Object not found", MB_ICONSTOP);
    _endthread();
  }
  ReadLine();
  while (Stuff != "")
  { // Read lines until EOF
    if (StrLeft(Stuff, 5) == "type:")
    { // 'type:' is found
      TmpStr = StrRight(Stuff, StrGetLength(Stuff) - 5);
      TmpStr = StrTrimLeft(TmpStr);
      if (TmpStr == Type)
      { // Type selected matches type for this object
        //*********
        //* Armor *
        //*********
        if (Type == "armor")
        { // Type is armor
          if (ArmorWear == "all")
          { // List all armor, no more checking is needed
            ObjectFile.close();
            return true;
          }
          ReadLine(); // ArmorValue
          ReadLine(); // ArmorWear
          TmpStr = StrRight(Stuff, StrGetLength(Stuff) - 10);
          TmpStr = StrTrimLeft(TmpStr);
          if (TmpStr == ArmorWear)
          { // ArmorWear matches
            ObjectFile.close();
            return true;
          }
          else
          { // ArmorWear does not match, reject it
            ObjectFile.close();
            return false;
          }
        }
        //**********
        //* Weapon *
        //**********
        if (Type == "weapon")
        { // Type is weapon
          if (WeaponType == "all")
          { // List all armor, no more checking is needed
            ObjectFile.close();
            return true;
          }
          ReadLine(); // WeaponType
          TmpStr = StrRight(Stuff, StrGetLength(Stuff) - 11);
          TmpStr = StrTrimLeft(TmpStr);
          if (TmpStr == WeaponType)
          { // WeaponType matches
            ObjectFile.close();
            return true;
          }
          else
          { // WeaponType does not match, reject it
            ObjectFile.close();
            return false;
          }
        }
        // Type matched and it wasn't rejected
        ObjectFile.close();
        return true;
      }
    }
    ReadLine();
  }
  ObjectFile.close();
  return false;
}

/***********************************************************
* Read a line from Object file                             *
************************************************************/

void BuildObjectsList::ReadLine()
{
  getline(ObjectFile, Stuff);
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  Stuff = StrMakeLower(Stuff);
}

/***********************************************************
* SetFilter                                                *
************************************************************/

void BuildObjectsList::SetFilter() 
{
  //********
  //* Type *
  //********
  if (TypeDropList == -1)
  {
    Type = "All";
  }
  else
  if (TypeDropList == 0)
  {
    Type = "All";
  }
  else
  if (TypeDropList == 1)
  {
    Type = "Armor";
  }
  else
  if (TypeDropList == 2)
  {
    Type = "Weapon";
  }
  else
  if (TypeDropList == 3)
  {
    Type = "Drink";
  }
  else
  if (TypeDropList == 4)
  {
    Type = "Food";
  }
  else
  if (TypeDropList == 5)
  {
    Type = "Junk";
  }
  else
  if (TypeDropList == 6)
  {
    Type = "NoTake";
  }
  else
  {
    Type = "NotImplementedYet";
  }
  Type = StrMakeLower(Type);
  //*************
  //* ArmorWear *
  //*************
  if (ArmorWearDropList == -1)
  {
    ArmorWear = "All";
  }
  else
  if (ArmorWearDropList == 0)
  {
    ArmorWear = "All";
  }
  else
  if (ArmorWearDropList == 1)
  {
    ArmorWear = "Head";
  }
  else
  if (ArmorWearDropList == 2)
  {
    ArmorWear = "Ear";
  }
  else
  if (ArmorWearDropList == 3)
  {
    ArmorWear = "Neck";
  }
  else
  if (ArmorWearDropList == 4)
  {
    ArmorWear = "Shoulders";
  }
  else
  if (ArmorWearDropList == 5)
  {
    ArmorWear = "Chest";
  }
  else
  if (ArmorWearDropList == 6)
  {
    ArmorWear = "Back";
  }
  else
  if (ArmorWearDropList == 7)
  {
    ArmorWear = "Arms";
  }
  else
  if (ArmorWearDropList == 8)
  {
    ArmorWear = "Wrist";
  }
  else
  if (ArmorWearDropList == 9)
  {
    ArmorWear = "Hands";
  }
  else
  if (ArmorWearDropList == 10)
  {
    ArmorWear = "Finger";
  }
  else
  if (ArmorWearDropList == 11)
  {
    ArmorWear = "Shield";
  }
  else
  if (ArmorWearDropList == 12)
  {
    ArmorWear = "Waist";
  }
  else
  if (ArmorWearDropList == 13)
  {
    ArmorWear = "Legs";
  }
  else
  if (ArmorWearDropList == 14)
  {
    ArmorWear = "Ankle";
  }
  else
  if (ArmorWearDropList == 15)
  {
    ArmorWear = "Feet";
  }
  ArmorWear = StrMakeLower(ArmorWear);
  //***************
  //* Weapon Type *
  //***************
  if (WeaponTypeDropList == -1)
  {
    WeaponType = "All";
  }
  else
  if (WeaponTypeDropList == 0)
  {
    WeaponType = "All";
  }
  else
  if (WeaponTypeDropList == 1)
  {
    WeaponType = "Axe";
  }
  else
  if (WeaponTypeDropList == 2)
  {
    WeaponType = "Club";
  }
  else
  if (WeaponTypeDropList == 3)
  {
    WeaponType = "Dagger";
  }
  else
  if (WeaponTypeDropList == 4)
  {
    WeaponType = "Hammer";
  }
  else
  if (WeaponTypeDropList == 5)
  {
    WeaponType = "Spear";
  }
  else
  if (WeaponTypeDropList == 6)
  {
    WeaponType = "Staff";
  }
  else
  if (WeaponTypeDropList == 7)
  {
    WeaponType = "Sword";
  }
  WeaponType = StrMakeLower(WeaponType);
}