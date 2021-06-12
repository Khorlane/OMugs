/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BuildMobiles.cpp                                 *
* Usage:  CRUD dialog for mobiles                          *
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
#include "BuildMobiles.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern BuildMobiles     *pBuildMobiles;
extern BuildMobilesList *pBuildMobilesList;

/***********************************************************
* BuildMobiles constructor                                 *
************************************************************/

BuildMobiles::BuildMobiles(CWnd *pParent /*=NULL*/)
	: CDialog(BuildMobiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(BuildMobiles)
	MobileId = _T("");
	Names = _T("");
	Talk = _T("");
	Level = _T("");
	Loot = _T("");
	HitPoints = _T("");
	ExpPoints = _T("");
	Desc2 = _T("");
	Desc1 = _T("");
	Damage = _T("");
	Armor = _T("");
	ActionAggro = FALSE;
	ActionDestroy = FALSE;
	ActionFaction = FALSE;
	ActionHelp = FALSE;
	ActionNoMove = FALSE;
	ActionNone = FALSE;
	ActionWimpy = FALSE;
	Desc3 = _T("");
	FactionRadio = -1;
	SexRadio = -1;
	AttackRadio = -1;
	//}}AFX_DATA_INIT

  // Create modeless dialog box window
  if (Create(BuildMobiles::IDD, pParent))
  {
    ShowWindow(SW_SHOW);
  }
}

/***********************************************************
* BuildMobiles dialog data exchanger                       *
************************************************************/

void BuildMobiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BuildMobiles)
	DDX_Text(pDX, IDC_MobileId, MobileId);
	DDX_Text(pDX, IDC_Names, Names);
	DDX_Text(pDX, IDC_Talk, Talk);
	DDX_Text(pDX, IDC_Level, Level);
	DDX_Text(pDX, IDC_Loot, Loot);
	DDX_Text(pDX, IDC_HitPoints, HitPoints);
	DDX_Text(pDX, IDC_ExpPoints, ExpPoints);
	DDX_Text(pDX, IDC_Desc2, Desc2);
	DDX_Text(pDX, IDC_Desc1, Desc1);
	DDX_Text(pDX, IDC_Damage, Damage);
	DDX_Text(pDX, IDC_Armor, Armor);
	DDX_Check(pDX, IDC_ActionAggro, ActionAggro);
	DDX_Check(pDX, IDC_ActionDestroy, ActionDestroy);
	DDX_Check(pDX, IDC_ActionFaction, ActionFaction);
	DDX_Check(pDX, IDC_ActionHelp, ActionHelp);
	DDX_Check(pDX, IDC_ActionNoMove, ActionNoMove);
	DDX_Check(pDX, IDC_ActionNone, ActionNone);
	DDX_Check(pDX, IDC_ActionWimpy, ActionWimpy);
	DDX_Text(pDX, IDC_Desc3, Desc3);
	DDX_Radio(pDX, IDC_Faction, FactionRadio);
	DDX_Radio(pDX, IDC_Sex, SexRadio);
	DDX_Radio(pDX, IDC_Attack, AttackRadio);
	//}}AFX_DATA_MAP
}

/***********************************************************
* BuildMobiles dialog message map                          *
************************************************************/

BEGIN_MESSAGE_MAP(BuildMobiles, CDialog)
	//{{AFX_MSG_MAP(BuildMobiles)
	ON_BN_CLICKED(IDLOOKUP, OnLookup)
	ON_BN_CLICKED(IDC_ActionNone, OnActionNone)
	ON_BN_CLICKED(IDC_ActionAggro, OnActionAggro)
	ON_BN_CLICKED(IDC_ActionFaction, OnActionFaction)
	ON_BN_CLICKED(IDC_ActionDestroy, OnActionDestroy)
	ON_BN_CLICKED(IDC_ActionHelp, OnActionHelp)
	ON_BN_CLICKED(IDC_ActionNoMove, OnActionNoMove)
	ON_BN_CLICKED(IDC_ActionWimpy, OnActionWimpy)
	ON_BN_CLICKED(IDSAVE, OnSave)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//==========================================================
//= Message handler functions                              =
//==========================================================

/***********************************************************
* OnAction check boxes                                     *
************************************************************/

void BuildMobiles::OnActionNone() 
{
  CButton *pActionNone    = (CButton*)GetDlgItem(IDC_ActionNone);
  CButton *pActionAggro   = (CButton*)GetDlgItem(IDC_ActionAggro);
  CButton *pActionFaction = (CButton*)GetDlgItem(IDC_ActionFaction);
  CButton *pActionDestroy = (CButton*)GetDlgItem(IDC_ActionDestroy);
  CButton *pActionHelp    = (CButton*)GetDlgItem(IDC_ActionHelp);
  CButton *pActionNoMove  = (CButton*)GetDlgItem(IDC_ActionNoMove);
  CButton *pActionWimpy   = (CButton*)GetDlgItem(IDC_ActionWimpy);
  
  if (pActionNone->GetCheck())
  {
    pActionAggro->SetCheck(0);
    pActionFaction->SetCheck(0);
    pActionDestroy->SetCheck(0);
    pActionHelp->SetCheck(0);
    pActionNoMove->SetCheck(0);
    pActionWimpy->SetCheck(0);
  }
}

void BuildMobiles::OnActionAggro() 
{
  CButton *pActionNone    = (CButton*)GetDlgItem(IDC_ActionNone);
  CButton *pActionAggro   = (CButton*)GetDlgItem(IDC_ActionAggro);
  
  if (pActionAggro->GetCheck())
  {
    pActionNone->SetCheck(0);
  }
}

void BuildMobiles::OnActionFaction() 
{
  CButton *pActionNone    = (CButton*)GetDlgItem(IDC_ActionNone);
  CButton *pActionFaction = (CButton*)GetDlgItem(IDC_ActionFaction);
  
  if (pActionFaction->GetCheck())
  {
    pActionNone->SetCheck(0);
  }
}

void BuildMobiles::OnActionDestroy() 
{
  CButton *pActionNone    = (CButton*)GetDlgItem(IDC_ActionNone);
  CButton *pActionDestroy = (CButton*)GetDlgItem(IDC_ActionDestroy);

  if (pActionDestroy->GetCheck())
  {
    pActionNone->SetCheck(0);
  }
}

void BuildMobiles::OnActionHelp() 
{
  CButton *pActionNone    = (CButton*)GetDlgItem(IDC_ActionNone);
  CButton *pActionHelp    = (CButton*)GetDlgItem(IDC_ActionHelp);

  if (pActionHelp->GetCheck())
  {
    pActionNone->SetCheck(0);
  }
}

void BuildMobiles::OnActionNoMove() 
{
  CButton *pActionNone    = (CButton*)GetDlgItem(IDC_ActionNone);
  CButton *pActionNoMove  = (CButton*)GetDlgItem(IDC_ActionNoMove);
	
  if (pActionNoMove->GetCheck())
  {
    pActionNone->SetCheck(0);
  }
}

void BuildMobiles::OnActionWimpy() 
{
  CButton *pActionNone    = (CButton*)GetDlgItem(IDC_ActionNone);
  CButton *pActionWimpy   = (CButton*)GetDlgItem(IDC_ActionWimpy);

  if (pActionWimpy->GetCheck())
  {
    pActionNone->SetCheck(0);
  }
}

/***********************************************************
* OnCancel                                                *
************************************************************/

void BuildMobiles::OnCancel() 
{	
	CDialog::OnClose();
  delete this;
  pBuildMobiles = NULL;
}

/***********************************************************
* OnClose                                                  *
************************************************************/

void BuildMobiles::OnClose() 
{	
	CDialog::OnClose();
  delete this;
  pBuildMobiles = NULL;
}

/***********************************************************
* OnDelete button handler                                  *
************************************************************/

void BuildMobiles::OnDelete() 
{
  MobileFileName  = MOBILES_DIR;
  MobileFileName += MobileId;
  MobileFileName += ".txt";
  Success = MobileFile.Open(MobileFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Mobile not found", MB_ICONSTOP);
    return;
  }
  MobileFile.Close();
  AfxMsgBox = AfxMessageBox("Delete " + MobileId + "?", MB_YESNO);
  if (AfxMsgBox == IDNO)
  {
    AfxMessageBox(MobileId + " was NOT deleted", MB_ICONSTOP);
    return;
  }
  CFile::Remove(MobileFileName);
}

/***********************************************************
* OnLookup button handler                                  *
************************************************************/

void BuildMobiles::OnLookup() 
{
  UpdateData(true); // Get data from dialog
  ParseStuff();
  UpdateData(false); // Put data to dialog
}

/***********************************************************
* OnOk                                                     *
************************************************************/

void BuildMobiles::OnOk() 
{	
	CDialog::OnClose();
  delete this;
  pBuildMobiles = NULL;
}

/***********************************************************
* OnSave button handler                                    *
************************************************************/

void BuildMobiles::OnSave() 
{
	UpdateData(true); // Get data from dialog
  WriteStuff();
}

//==========================================================
//= Message handler support functions                      =
//==========================================================

/***********************************************************
* Get selected mobile                                      *
************************************************************/

void BuildMobiles::GetSelectedMob()
{
  pBuildMobiles->MobileId = pBuildMobilesList->MobileId;
  ParseStuff();
  UpdateData(false); // Put data to dialog
}

/***********************************************************
* Parse mobile stuff                                       *
************************************************************/

void BuildMobiles::ParseStuff()
{
  MobileFileName =  MOBILES_DIR;
  MobileFileName += MobileId;
  MobileFileName += ".txt";
  Success = MobileFile.Open(MobileFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  { // Mobile not found
    AfxMessageBox("Mobile not found", MB_ICONSTOP);
    return;
  }
  ReadLine();
  while (Stuff != "")
  {
    // MobileId
    if (Stuff.Left(9) == "MobileId:")
    {
      MobileId = Stuff.Right(Stuff.GetLength()-9);
      MobileId.TrimLeft();
    }
    else
    // Names
    if (Stuff.Left(6) == "Names:")
    {
      Names = Stuff.Right(Stuff.GetLength()-6);
      Names.TrimLeft();
    }
    else
    // Sex
    if (Stuff.Left(4) == "Sex:")
    {
      Sex = Stuff.Right(Stuff.GetLength()-4);
      Sex.TrimLeft();
      if (Sex == "M")
      {
        SexRadio = 0;
      }
      else
      if (Sex == "F")
      {
        SexRadio = 1;
      }
      else
      {
        SexRadio = 2;
      }
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
      MobileFile.ReadString(Stuff); // Do not use Readline()
      while (Stuff != "End Desc3")
      {
        Desc3 += Stuff;
        Desc3 += "\r\n";
        MobileFile.ReadString(Stuff); // Do not use Readline()
      }
      Desc3 = Desc3.Left(Desc3.GetLength()-2);
    }
    else
    // Action
    if (Stuff.Left(7) == "Action:")
    {
      ActionNone    = false;
      ActionAggro   = false;
      ActionFaction = false;
      ActionDestroy = false;
      ActionHelp    = false;
      ActionNoMove  = false;
      ActionWimpy   = false;
      Action = Stuff.Right(Stuff.GetLength()-7);
      Action.TrimLeft();
      if (Action.Find("None") > -1)
      {
        ActionNone = true;
      }
      if (Action.Find("Aggro") > -1)
      {
        ActionAggro = true;
      }
      if (Action.Find("Faction") > -1)
      {
        ActionFaction = true;
      }
      if (Action.Find("Destroy") > -1)
      {
        ActionDestroy = true;
      }
      if (Action.Find("Help") > -1)
      {
        ActionHelp = true;
      }
      if (Action.Find("NoMove") > -1)
      {
        ActionNoMove = true;
      }
      if (Action.Find("Wimpy") > -1)
      {
        ActionWimpy = true;
      }
    }
    else
    // Faction
    if (Stuff.Left(8) == "Faction:")
    {
      Faction = Stuff.Right(Stuff.GetLength()-8);
      Faction.TrimLeft();
      if (Faction.Find("Good") > -1)
      {
        FactionRadio = 0;
      }
      else
      if (Faction.Find("Lawful") > -1)
      {
        FactionRadio = 1;
      }
      else
      if (Faction.Find("Neutral") > -1)
      {
        FactionRadio = 2;
      }
      else
      if (Faction.Find("Lawless") > -1)
      {
        FactionRadio = 3;
      }
      else
      if (Faction.Find("Evil") > -1)
      {
        FactionRadio = 4;
      }
    }
    else
    // Level
    if (Stuff.Left(6) == "Level:")
    {
      Level = Stuff.Right(Stuff.GetLength()-6);
      Level.TrimLeft();
    }
    else
    // HitPoints
    if (Stuff.Left(10) == "HitPoints:")
    {
      HitPoints  = Stuff.Right(Stuff.GetLength()-10);
      HitPoints.TrimLeft();
    }
    else
    // Armor
    if (Stuff.Left(6) == "Armor:")
    {
      Armor  = Stuff.Right(Stuff.GetLength()-6);
      Armor.TrimLeft();
    }
    else
    // Attack
    if (Stuff.Left(7) == "Attack:")
    {
      Attack = Stuff.Right(Stuff.GetLength()-7);
      Attack.TrimLeft();
      if (Attack.Find("Bites") > -1)
      {
        AttackRadio = 0;
      }
      else
      if (Attack.Find("Claws") > -1)
      {
        AttackRadio = 1;
      }
      else
      if (Attack.Find("Crushes") > -1)
      {
        AttackRadio = 2;
      }
      else
      if (Attack.Find("Hits") > -1)
      {
        AttackRadio = 3;
      }
      else
      if (Attack.Find("Mauls") > -1)
      {
        AttackRadio = 4;
      }
      else
      if (Attack.Find("Pierces") > -1)
      {
        AttackRadio = 5;
      }
      else
      if (Attack.Find("Punches") > -1)
      {
        AttackRadio = 6;
      }
      else
      if (Attack.Find("Slashes") > -1)
      {
        AttackRadio = 7;
      }
      else
      if (Attack.Find("Stabs") > -1)
      {
        AttackRadio = 8;
      }
      else
      if (Attack.Find("Stings") > -1)
      {
        AttackRadio = 9;
      }
      else
      if (Attack.Find("Thrashes") > -1)
      {
        AttackRadio = 10;
      }
    }
    else
    // Damage
    if (Stuff.Left(7) == "Damage:")
    {
      Damage  = Stuff.Right(Stuff.GetLength()-7);
      Damage.TrimLeft();
    }
    else
    if (Stuff.Left(10) == "ExpPoints:")
    {
      ExpPoints  = Stuff.Right(Stuff.GetLength()-10);
      ExpPoints.TrimLeft();
    }
    else
    // Loot
    if (Stuff.Left(5) == "Loot:")
    {
      Loot = Stuff.Right(Stuff.GetLength()-5);
      Loot.TrimLeft();
    }
    else
    // Talk
    if (Stuff.Left(5) == "Talk:")
    {
      Talk = Stuff.Right(Stuff.GetLength()-5);
      Talk.TrimLeft();
    }
    ReadLine();
  }
  MobileFile.Close();
}

/***********************************************************
* Read a line from Mobile file                             *
************************************************************/

void BuildMobiles::ReadLine()
{
  MobileFile.ReadString(Stuff);
  Stuff.TrimLeft();
  Stuff.TrimRight();
}

/***********************************************************
* Write mobile stuff                                       *
************************************************************/

void BuildMobiles::WriteStuff()
{
  MobileFileName =  MOBILES_DIR;
  MobileFileName += MobileId;
  MobileFileName += ".txt";
  Success = MobileFile.Open(MobileFileName,
                 CFile::modeCreate |
                 CFile::modeWrite  |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Mobile not found", MB_ICONSTOP);
    return;
  }
  // Create Mobile stuff string
  Stuff  = "";
  // MobileId
  Stuff += "MobileId:  ";
  Stuff += MobileId;
  Stuff += "\n";
  // Names
  Stuff += "Names:     ";
  Stuff += Names;
  Stuff += "\n";
  // Sex
  Stuff += "Sex:       ";
  if (SexRadio == 0)
  {
    Stuff += "M";
  }
  else
  if (SexRadio == 1)
  {
    Stuff += "F";
  }
  else
  if (SexRadio == 2)
  {
    Stuff += "N";
  }
  Stuff += "\n";
  // Desc1
  Stuff += "Desc1:     ";
  Stuff += Desc1;
  Stuff += "\n";
  // Desc2
  Stuff += "Desc2:     ";
  Stuff += Desc2;
  Stuff += "\n";
  // Desc3
  Stuff += "Desc3:";
  Stuff += "\n";
  Stuff += Desc3;
  Stuff += "\n";
  Stuff += "End Desc3";
  Stuff += "\n";
  // Action
  Stuff += "Action:    ";
  if (ActionNone)
  {
    Stuff += "None ";
  }
  if (ActionAggro)
  {
    Stuff += "Aggro ";
  }
  if (ActionFaction)
  {
    Stuff += "Faction ";
  }
  if (ActionDestroy)
  {
    Stuff += "Destroy ";
  }
  if (ActionHelp)
  {
    Stuff += "Help ";
  }
  if (ActionNoMove)
  {
    Stuff += "NoMove ";
  }
  if (ActionWimpy)
  {
    Stuff += "Wimpy ";
  }
  Stuff.TrimRight();
  Stuff += "\n";
  // Faction
  Stuff += "Faction:   ";
  if (FactionRadio == 0)
  {
    Stuff += "Good";
  }
  else
  if (FactionRadio == 1)
  {
    Stuff += "Lawful";
  }
  else
  if (FactionRadio == 2)
  {
    Stuff += "Neutral";
  }
  else
  if (FactionRadio == 3)
  {
    Stuff += "Lawless";
  }
  else
  if (FactionRadio == 4)
  {
    Stuff += "Evil";
  }
  Stuff += "\n";
  // Level
  Stuff += "Level:     ";
  Stuff += Level;
  Stuff += "\n";
  // HitPoints
  Stuff += "HitPoints: ";
  Stuff += HitPoints;
  Stuff += "\n";
  // Armor
  Stuff += "Armor:     ";
  Stuff += Armor;
  Stuff += "\n";
  // Attack
  Stuff += "Attack:    ";
  if (AttackRadio == 0)
  {
    Stuff += "Bites";
  }
  else
  if (AttackRadio == 1)
  {
    Stuff += "Claws";
  }
  else
  if (AttackRadio == 2)
  {
    Stuff += "Crushes";
  }
  else
  if (AttackRadio == 3)
  {
    Stuff += "Hits";
  }
  else
  if (AttackRadio == 4)
  {
    Stuff += "Mauls";
  }
  else
  if (AttackRadio == 5)
  {
    Stuff += "Pierces";
  }
  else
  if (AttackRadio == 6)
  {
    Stuff += "Punches";
  }
  else
  if (AttackRadio == 7)
  {
    Stuff += "Slashes";
  }
  else
  if (AttackRadio == 8)
  {
    Stuff += "Stabs";
  }
  else
  if (AttackRadio == 9)
  {
    Stuff += "Stings";
  }
  else
  if (AttackRadio == 10)
  {
    Stuff += "Thrashes";
  }
  Stuff += "\n";
  // Damage
  Stuff += "Damage:    ";
  Stuff += Damage;
  Stuff += "\n";
  // ExpPoints
  Stuff += "ExpPoints: ";
  Stuff += ExpPoints;
  Stuff += "\n";
  // Loot
  Stuff += "Loot:      ";
  Stuff += Loot;
  Stuff += "\n";
  // Talk
  Stuff += "Talk:      ";
  Stuff += Talk;
  Stuff += "\n";
  // Write it all out and close file
  MobileFile.WriteString(Stuff);
  MobileFile.Close();
}