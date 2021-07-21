/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildObjects.cpp                                 *
* Usage:  CRUD dialog for objects                          *
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
#include "BuildObjects.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* BuildObjects constructor                                 *
************************************************************/

BuildObjects::BuildObjects(CWnd* pParent /*=NULL*/)
	: CDialog(BuildObjects::IDD, pParent)
{
	//{{AFX_DATA_INIT(BuildObjects)
	Cost = _T("");
	Desc1 = _T("");
	Desc2 = _T("");
	Desc3 = _T("");
	DrinkPct = _T("");
	FoodPct = _T("");
	Names = _T("");
	ObjectId = _T("");
	Weight = _T("");
	WeaponDamage = _T("");
	ArmorValue = _T("");
	ArmorWearDropList = -1;
	TypeDropList = -1;
	WeaponTypeDropList = -1;
	//}}AFX_DATA_INIT

  // Create modeless dialog box window
  if (Create(BuildObjects::IDD, pParent))
  {
    ShowWindow(SW_SHOW);
  }
}

/***********************************************************
* BuildObjects dialog data exchanger                       *
************************************************************/

void BuildObjects::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BuildObjects)
	DDX_Text(pDX, IDC_Cost, Cost);
	DDX_Text(pDX, IDC_Desc1, Desc1);
	DDX_Text(pDX, IDC_Desc2, Desc2);
	DDX_Text(pDX, IDC_Desc3, Desc3);
	DDX_Text(pDX, IDC_DrinkPct, DrinkPct);
	DDX_Text(pDX, IDC_FoodPct, FoodPct);
	DDX_Text(pDX, IDC_Names, Names);
	DDX_Text(pDX, IDC_ObjectId, ObjectId);
	DDX_Text(pDX, IDC_Weight, Weight);
	DDX_Text(pDX, IDC_WeaponDamage, WeaponDamage);
	DDX_Text(pDX, IDC_ArmorValue, ArmorValue);
	DDX_CBIndex(pDX, IDC_ArmorWear, ArmorWearDropList);
	DDX_CBIndex(pDX, IDC_Type, TypeDropList);
	DDX_CBIndex(pDX, IDC_WeaponType, WeaponTypeDropList);
	//}}AFX_DATA_MAP
}

/***********************************************************
* BuildObjects dialog message map                          *
************************************************************/

BEGIN_MESSAGE_MAP(BuildObjects, CDialog)
	//{{AFX_MSG_MAP(BuildObjects)
	ON_BN_CLICKED(IDLOOKUP, OnLookup)
	ON_BN_CLICKED(IDSAVE, OnSave)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//==========================================================
//= Message handler functions                              =
//==========================================================

/***********************************************************
* OnCancel                                                *
************************************************************/

void BuildObjects::OnCancel() 
{	
	CDialog::OnClose();
  delete this;
  pBuildObjects = NULL;
}

/***********************************************************
* OnClose                                                  *
************************************************************/

void BuildObjects::OnClose() 
{	
	CDialog::OnClose();
  delete this;
  pBuildObjects = NULL;
}

/***********************************************************
* OnDelete button handler                                  *
************************************************************/

void BuildObjects::OnDelete() 
{
  ObjectFileName  = OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  Success = ObjectFile.Open(ObjectFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object not found", MB_ICONSTOP);
    return;
  }
  ObjectFile.Close();
  AfxMsgBox = AfxMessageBox("Delete " + ObjectId + "?", MB_YESNO);
  if (AfxMsgBox == IDNO)
  {
    AfxMessageBox(ObjectId + " was NOT deleted", MB_ICONSTOP);
    return;
  }
  CFile::Remove(ObjectFileName);
}

/***********************************************************
* OnLookup button handler                                  *
************************************************************/

void BuildObjects::OnLookup() 
{
  UpdateData(true); // Get data from dialog
  ParseStuff();
  UpdateData(false); // Put data to dialog	
}

/***********************************************************
* OnOk                                                     *
************************************************************/

void BuildObjects::OnOk() 
{	
	CDialog::OnClose();
  delete this;
  pBuildObjects = NULL;
}

/***********************************************************
* OnSave button handler                                    *
************************************************************/

void BuildObjects::OnSave() 
{
	UpdateData(true); // Get data from dialog
  WriteStuff();
}

//==========================================================
//= Message handler support functions                      =
//==========================================================

/***********************************************************
* Get selected object                                      *
************************************************************/

void BuildObjects::GetSelectedObj()
{
  pBuildObjects->ObjectId = pBuildObjectsList->ObjectId;
  ParseStuff();
  UpdateData(false); // Put data to dialog
}

/***********************************************************
* Parse object stuff                                       *
************************************************************/

void BuildObjects::ParseStuff()
{
  ObjectFileName =  OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  Success = ObjectFile.Open(ObjectFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  { // Mobile not found
    AfxMessageBox("Object not found", MB_ICONSTOP);
    return;
  }
  ArmorWearDropList  = -1;
  WeaponTypeDropList = -1;
  ArmorWear          = "";
  ArmorValue         = "";
  WeaponType         = "";
  WeaponDamage       = "";
  DrinkPct           = "";
  FoodPct            = "";
  ReadLine();
  while (Stuff != "")
  {
    // ObjectId
    if (Stuff.Left(9) == "ObjectId:")
    {
      ObjectId = Stuff.Right(Stuff.GetLength()-9);
      ObjectId.TrimLeft();
    }
    else
    // Names
    if (Stuff.Left(6) == "Names:")
    {
      Names = Stuff.Right(Stuff.GetLength()-6);
      Names.TrimLeft();
    }
    else
    // Desc1
    if (Stuff.Left(6) == "Desc1:")
    {
      Desc1 = Stuff.Right(Stuff.GetLength()-6);
      Desc1.TrimLeft();
    }
    else
    // Desc2
    if (Stuff.Left(6) == "Desc2:")
    {
      Desc2 = Stuff.Right(Stuff.GetLength()-6);
      Desc2.TrimLeft();
    }
    else
    // Desc3
    if (Stuff.Left(6) == "Desc3:")
    { 
      Desc3 = "";
      ObjectFile.ReadString(Stuff); // Do not use Readline()
      while (Stuff != "End Desc3")
      {
        Desc3 += Stuff;
        Desc3 += "\r\n";
        ObjectFile.ReadString(Stuff); // Do not use Readline()
      }
      Desc3 = Desc3.Left(Desc3.GetLength()-2);
    }
    else
    // Weight
    if (Stuff.Left(7) == "Weight:")
    {
      Weight = Stuff.Right(Stuff.GetLength()-7);
      Weight.TrimLeft();
    }
    else
    // Cost
    if (Stuff.Left(5) == "Cost:")
    {
      Cost = Stuff.Right(Stuff.GetLength()-5);
      Cost.TrimLeft();
    }
    else
    // Type
    if (Stuff.Left(5) == "Type:")
    {
      Type = Stuff.Right(Stuff.GetLength()-5);
      Type.TrimLeft();
      // Armor
      if (Type == "Armor")
      {
        TypeDropList = 0;
        // ArmorValue
        ReadLine();
        if (Stuff.Left(11) != "ArmorValue:")
        { // Expecting ArmorValue
          AfxMessageBox("Expecting ArmorValue", MB_ICONSTOP);
          ObjectFile.Close();
          return;
        }
        ArmorValue = Stuff.Right(Stuff.GetLength()-11);
        ArmorValue.TrimLeft();
        // ArmorWear
        ReadLine();
        if (Stuff.Left(10) != "ArmorWear:")
        { // Expecting ArmorValue
          AfxMessageBox("Expecting ArmorWear", MB_ICONSTOP);
          ObjectFile.Close();
          return;
        }
        ArmorWear = Stuff.Right(Stuff.GetLength()-10);
        ArmorWear.TrimLeft();
        if (ArmorWear == "Head")
        {
          ArmorWearDropList = 0;
        }
        else
        if (ArmorWear == "Ear")
        {
          ArmorWearDropList = 1;
        }
        else
        if (ArmorWear == "Neck")
        {
          ArmorWearDropList = 2;
        }
        else
        if (ArmorWear == "Shoulders")
        {
          ArmorWearDropList = 3;
        }
        else
        if (ArmorWear == "Chest")
        {
          ArmorWearDropList = 4;
        }
        else
        if (ArmorWear == "Back")
        {
          ArmorWearDropList = 5;
        }
        else
        if (ArmorWear == "Arms")
        {
          ArmorWearDropList = 6;
        }
        else
        if (ArmorWear == "Wrist")
        {
          ArmorWearDropList = 7;
        }
        else
        if (ArmorWear == "Hands")
        {
          ArmorWearDropList = 8;
        }
        else
        if (ArmorWear == "Finger")
        {
          ArmorWearDropList = 9;
        }
        else
        if (ArmorWear == "Shield")
        {
          ArmorWearDropList = 10;
        }
        else
        if (ArmorWear == "Waist")
        {
          ArmorWearDropList = 11;
        }
        else
        if (ArmorWear == "Legs")
        {
          ArmorWearDropList = 12;
        }
        else
        if (ArmorWear == "Ankle")
        {
          ArmorWearDropList = 13;
        }
        else
        if (ArmorWear == "Feet")
        {
          ArmorWearDropList = 14;
        }
      }
      else
      // Weapon
      if (Type == "Weapon")
      {
        TypeDropList = 1;
        // WeaponType
        ReadLine();
        if (Stuff.Left(11) != "WeaponType:")
        { // Expecting WeaponType
          AfxMessageBox("Expecting WeaponType", MB_ICONSTOP);
          ObjectFile.Close();
          return;
        }
        WeaponType = Stuff.Right(Stuff.GetLength()-11);
        WeaponType.TrimLeft();
        if (WeaponType == "Axe")
        {
          WeaponTypeDropList = 0;
        }
        else
        if (WeaponType == "Club")
        {
          WeaponTypeDropList = 1;
        }
        else
        if (WeaponType == "Dagger")
        {
          WeaponTypeDropList = 2;
        }
        else
        if (WeaponType == "Hammer")
        {
          WeaponTypeDropList = 3;
        }
        else
        if (WeaponType == "Spear")
        {
          WeaponTypeDropList = 4;
        }
        else
        if (WeaponType == "Staff")
        {
          WeaponTypeDropList = 5;
        }
        else
        if (WeaponType == "Sword")
        {
          WeaponTypeDropList = 6;
        }
        // WeaponDamage
        ReadLine();
        if (Stuff.Left(13) != "WeaponDamage:")
        { // Expecting WeaponDamage
          AfxMessageBox("Expecting WeaponDamage", MB_ICONSTOP);
          ObjectFile.Close();
          return;
        }
        WeaponDamage = Stuff.Right(Stuff.GetLength()-13);
        WeaponDamage.TrimLeft();
      }
      else
      // Drink
      if (Type == "Drink")
      {
        TypeDropList = 2;
        // DrinkPct
        ReadLine();
        if (Stuff.Left(9) != "DrinkPct:")
        { // Expecting DrinkPct
          AfxMessageBox("Expecting DrinkPct", MB_ICONSTOP);
          ObjectFile.Close();
          return;
        }
        DrinkPct = Stuff.Right(Stuff.GetLength()-9);
        DrinkPct.TrimLeft();
      }
      else
      // Food
      if (Type == "Food")
      {
        TypeDropList = 3;
        // FoodPct
        ReadLine();
        if (Stuff.Left(8) != "FoodPct:")
        { // Expecting FoodPct
          AfxMessageBox("Expecting FoodPct", MB_ICONSTOP);
          ObjectFile.Close();
          return;
        }
        FoodPct = Stuff.Right(Stuff.GetLength()-8);
        FoodPct.TrimLeft();
      }
      else
      // Junk
      if (Type == "Junk")
      {
        TypeDropList = 4;
      }
      else
      // NoTake
      if (Type == "NoTake")
      {
        TypeDropList = 5;
      }
    }
    ReadLine();
  }
  ObjectFile.Close();
}


/***********************************************************
* Read a line from Object file                             *
************************************************************/

void BuildObjects::ReadLine()
{
  ObjectFile.ReadString(Stuff);
  Stuff.TrimLeft();
  Stuff.TrimRight();
}

/***********************************************************
* Write object stuff                                       *
************************************************************/

void BuildObjects::WriteStuff()
{
  // Create Object stuff string
  Stuff  = "";
  // ObjectId
  Stuff += "ObjectId:     ";
  Stuff += ObjectId;
  Stuff += "\n";
  // Names
  Stuff += "Names:        ";
  Stuff += Names;
  Stuff += "\n";
  // Desc1
  Stuff += "Desc1:        ";
  Stuff += Desc1;
  Stuff += "\n";
  // Desc2
  Stuff += "Desc2:        ";
  Stuff += Desc2;
  Stuff += "\n";
  // Desc3
  Stuff += "Desc3:";
  Stuff += "\n";
  Stuff += Desc3;
  Stuff += "\n";
  Stuff += "End Desc3";
  Stuff += "\n";
  // Weight
  Stuff += "Weight:       ";
  Stuff += Weight;
  Stuff += "\n";
  // Cost
  Stuff += "Cost:         ";
  Stuff += Cost;
  Stuff += "\n";
  // Type
  Type = "";
  if (TypeDropList == 0)
  {
    Type = "Armor";
  }
  else
  if (TypeDropList == 1)
  {
    Type = "Weapon";
  }
  else
  if (TypeDropList == 2)
  {
    Type = "Drink";
  }
  else
  if (TypeDropList == 3)
  {
    Type = "Food";
  }
  else
  if (TypeDropList == 4)
  {
    Type = "Junk";
  }
  else
  if (TypeDropList == 5)
  {
    Type = "NoTake";
  }
  if (Type == "")
  { // Something is wrong
    AfxMessageBox("Object type not implemented yet", MB_ICONSTOP);
    return;
  }
  Stuff += "Type:         ";
  Stuff += Type;
  // Armor
  if (Type == "Armor")
  { // ArmorValue
    Stuff += "\n";
    Stuff += "ArmorValue:   ";
    Stuff += ArmorValue;
    Stuff += "\n";
    // ArmorWear
    if (ArmorWearDropList == 0)
    {
      ArmorWear = "Head";
    }
    else
    if (ArmorWearDropList == 1)
    {
      ArmorWear = "Ear";
    }
    else
    if (ArmorWearDropList == 2)
    {
      ArmorWear = "Neck";
    }
    else
    if (ArmorWearDropList == 3)
    {
      ArmorWear = "Shoulders";
    }
    else
    if (ArmorWearDropList == 4)
    {
      ArmorWear = "Chest";
    }
    else
    if (ArmorWearDropList == 5)
    {
      ArmorWear = "Back";
    }
    else
    if (ArmorWearDropList == 6)
    {
      ArmorWear = "Arms";
    }
    else
    if (ArmorWearDropList == 7)
    {
      ArmorWear = "Wrist";
    }
    else
    if (ArmorWearDropList == 8)
    {
      ArmorWear = "Hands";
    }
    else
    if (ArmorWearDropList == 9)
    {
      ArmorWear = "Finger";
    }
    else
    if (ArmorWearDropList == 10)
    {
      ArmorWear = "Shield";
    }
    else
    if (ArmorWearDropList == 11)
    {
      ArmorWear = "Waist";
    }
    else
    if (ArmorWearDropList == 12)
    {
      ArmorWear = "Legs";
    }
    else
    if (ArmorWearDropList == 13)
    {
      ArmorWear = "Ankle";
    }
    else
    if (ArmorWearDropList == 14)
    {
      ArmorWear = "Feet";
    }
    Stuff += "ArmorWear:    ";
    Stuff += ArmorWear;
  }
  else
  // Weapon
  if (Type == "Weapon")
  {
    if (WeaponTypeDropList == 0)
    {
      WeaponType = "Axe";
    }
    else
    if (WeaponTypeDropList == 1)
    {
      WeaponType = "Club";
    }
    else
    if (WeaponTypeDropList == 2)
    {
      WeaponType = "Dagger";
    }
    else
    if (WeaponTypeDropList == 3)
    {
      WeaponType = "Hammer";
    }
    else
    if (WeaponTypeDropList == 4)
    {
      WeaponType = "Spear";
    }
    else
    if (WeaponTypeDropList == 5)
    {
      WeaponType = "Staff";
    }
    else
    if (WeaponTypeDropList == 6)
    {
      WeaponType = "Sword";
    }
    // WeaponType
    Stuff += "\n";
    Stuff += "WeaponType:   ";
    Stuff += WeaponType;
    Stuff += "\n";
    // WeaponDamage
    Stuff += "WeaponDamage: ";
    Stuff += WeaponDamage;
  }
  else
  // DrinkPct
  if (Type == "Drink")
  {
    Stuff += "\n";
    Stuff += "DrinkPct:     ";
    Stuff += DrinkPct;
  }
  else
  // FoodPct
  if (Type == "Food")
  {
    Stuff += "\n";  
    Stuff += "FoodPct:      ";
    Stuff += FoodPct;
  }
  // Open the file, write it all out, and close file
  ObjectFileName =  OBJECTS_DIR;
  ObjectFileName += ObjectId;
  ObjectFileName += ".txt";
  Success = ObjectFile.Open(ObjectFileName,
                 CFile::modeCreate |
                 CFile::modeWrite  |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Object not found", MB_ICONSTOP);
    return;
  }
  ObjectFile.WriteString(Stuff);
  ObjectFile.Close();
}