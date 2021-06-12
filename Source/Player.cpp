/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Player.cpp                                       *
* Usage:  Manage Players                                   *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Player.h"

/***********************************************************
* Global                                                   *
************************************************************/

int     Player::Count = 0;
int     Player::GetCount() {return Count;}

/***********************************************************
* Constructor                                              *
************************************************************/

Player::Player()
{
  Count++;
  PlayerId            = Count;
  Name                = "";
  Password            = "";
  Level               = 1;
  Experience          = 0;
  Sex                 = "";
  Copper              = 0;
  Bronze              = 0;
  Silver              = 0;
  Gold                = 0;
  HitPoints           = 15;
  MovePoints          = 25;
  Position            = "stand";
  RoomId              = "JesseSquare";
  Color               = false;
  Group               = false;
  Title               = "";
  for (int i=0;i<GRP_LIMIT;i++)
  {
    pPlayerGrpMember[i] = NULL;
    pPlayerFollowers[i] = NULL;
  }
}

/***********************************************************
* Destructor                                               *
************************************************************/

Player::~Player()
{
  Count--;
}

/***********************************************************
* Manipulate player money                                  *
************************************************************/

void Player::SetMoney(char PlusMinus, int Amount, CString Metal)
{
  if (PlusMinus == '-')
    Amount = Amount * -1;

  if (Metal == "Gold")
  {
    Gold = Gold + Amount;
  }
  else
  if (Metal == "Silver")
  {
    Silver = Silver + Amount;
  }
  else
  if (Metal == "Bronze")
  {
    Bronze = Bronze + Amount;
  }
  else
  if (Metal == "Copper")
  {
    Copper = Copper + Amount;
  }
  else
  {
    exit(-1);
  }
}

/***********************************************************
* Show player money                                        *
************************************************************/

void Player::ShowMoney()
{
  CString TmpStr;
  Output = "\r\n";

  TmpStr.Format("%d", Gold);
  Output += "Gold:   " + TmpStr + "\r\n";
  
  TmpStr.Format("%d", Silver);
  Output += "Silver: " + TmpStr + "\r\n";

  TmpStr.Format("%d", Bronze);
  Output += "Bronze: " + TmpStr + "\r\n";

  TmpStr.Format("%d", Copper);
  Output += "Copper: " + TmpStr + "\r\n";

}

/***********************************************************
* Save player stuff                                        *
************************************************************/

void Player::Save()
{
  CString TmpStr;

  if (!OpenFile(Name, "Write"))
  {
    AfxMessageBox("Error writing player file, Players directory may not exist");
    exit(-1);
  }

  Stuff = "Name:" + Name;
  WriteLine(Stuff);

  Stuff = "Password:" + Password;
  WriteLine(Stuff);

  TmpStr.Format("%d", Level);
  Stuff = "Level:" + TmpStr;
  WriteLine(Stuff);

  TmpStr.Format("%d", Experience);
  Stuff = "Experience:" + TmpStr;
  WriteLine(Stuff);

  Stuff = "RoomId:" + RoomId;
  WriteLine(Stuff);

  Stuff = "Sex:" + Sex;
  WriteLine(Stuff);

  TmpStr.Format("%d", Gold);
  Stuff = "Gold:" + TmpStr;
  WriteLine(Stuff);

  TmpStr.Format("%d", Silver);
  Stuff = "Silver:" + TmpStr;
  WriteLine(Stuff);

  TmpStr.Format("%d", Bronze);
  Stuff = "Bronze:" + TmpStr;
  WriteLine(Stuff);

  TmpStr.Format("%d", Copper);
  Stuff = "Copper:" + TmpStr;
  WriteLine(Stuff);

  if (Color)
  {
    Stuff = "Color:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "Color:No";
    WriteLine(Stuff);
  }

  if (Group)
  {
    Stuff = "Group:Yes";
    WriteLine(Stuff);
  }
  else
  {
    Stuff = "Group:No";
    WriteLine(Stuff);
  }

  Stuff = "Title:" + Title;
  WriteLine(Stuff);

  Stuff = "Position:" + Position;
  WriteLine(Stuff);

  CloseFile();
}

/***********************************************************
* Write a line to player file                              *
************************************************************/

void Player::WriteLine(CString Stuff)
{
  Stuff = Stuff + "\n";
  PlayerFile.WriteString(Stuff);
  PlayerFile.Flush();
}

/***********************************************************
* Validate player name                                     *
************************************************************/

bool Player::ValidName(CString Name)
{
  int Success;
  CStdioFile  ValidNameFile;
  CString FileName = ".\\Players\\ValidNames.txt";
  CString NameIn;

  Success = ValidNameFile.Open(FileName,
            CFile::modeRead |
            CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Error opening valid name file, it may not exist");
    exit(-1);
  }
  ValidNameFile.ReadString(NameIn);
  while (NameIn != "")
  {
    if (Name == NameIn)
    {
      ValidNameFile.Close();
      return true;
    }
    ValidNameFile.ReadString(NameIn);
  }
  ValidNameFile.Close();
  return false;
}

/***********************************************************
* Open player file                                         *
************************************************************/

bool Player::OpenFile(CString Name, CString Mode)
{
  int Success;
  CString FileName = ".\\Players\\" + Name + ".txt";

  if (Mode == "Read")
  {
    Success = PlayerFile.Open(FileName,
              CFile::modeRead |
              CFile::typeText);
    if(!Success)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  if (Mode == "Write")
  {
    Success = PlayerFile.Open(FileName,
              CFile::modeCreate |
              CFile::modeWrite  |
              CFile::typeText);
    if(!Success)
    {
      return false;
    }
    else
    {
      return true;
    }
  }
  else
  {
    exit(-1);
  }
}

/***********************************************************
* Read a line from player file                             *
************************************************************/

CString Player::ReadLine()
{
  PlayerFile.ReadString(Stuff);
  return Stuff; // fixme -- don't need to return stuff
}

/***********************************************************
* Close player file                                        *
************************************************************/

void Player::CloseFile()
{
  PlayerFile.Close();
}

/***********************************************************
* Parse player stuff                                       *
************************************************************/

void Player::ParseStuff(CString Name)
{
  int     Amount;
  CString TmpStr;

  Stuff = "Not Done Yet";
  OpenFile(Name, "Read");
  ReadLine();
  while (Stuff != "")
  {
    if (Stuff.Left(9) == "Password:")
    {
      Password = Stuff.Right(Stuff.GetLength()-9);
    }
    else
    if (Stuff.Left(6) == "Level:")
    {
      Level = atoi(Stuff.Right(Stuff.GetLength()-6));
    }
    else
    if (Stuff.Left(11) == "Experience:")
    {
      Experience = atoi(Stuff.Right(Stuff.GetLength()-11));
    }
    else
    if (Stuff.Left(7) == "RoomId:")
    {
      RoomId = Stuff.Right(Stuff.GetLength()-7);
    }
    else
    if (Stuff.Left(4) == "Sex:")
    {
      Sex = Stuff.Right(Stuff.GetLength()-4);
    }
    else
    if (Stuff.Left(5) == "Gold:")
    {
      Amount = atoi(Stuff.Right(Stuff.GetLength()-5));
      SetMoney('+', Amount, "Gold");
    }
    else
    if (Stuff.Left(7) == "Silver:")
    {
      Amount = atoi(Stuff.Right(Stuff.GetLength()-7));
      SetMoney('+', Amount, "Silver");
    }
    else
    if (Stuff.Left(7) == "Bronze:")
    {
      Amount = atoi(Stuff.Right(Stuff.GetLength()-7));
      SetMoney('+', Amount, "Bronze");
    }
    else
    if (Stuff.Left(7) == "Copper:")
    {
      Amount = atoi(Stuff.Right(Stuff.GetLength()-7));
      SetMoney('+', Amount, "Copper");
    }
    else
    if (Stuff.Left(6) == "Color:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-6);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        Color = true;
      }
      else
      {
        Color = false;
      }
    }
    else
    if (Stuff.Left(6) == "Group:")
    {
      TmpStr = Stuff.Right(Stuff.GetLength()-6);
      TmpStr.MakeLower();
      if (TmpStr == "yes")
      {
        Group = true;
      }
      else
      {
        Group = false;
      }
    }
    else
    if (Stuff.Left(6) == "Title:")
    {
      Title = Stuff.Right(Stuff.GetLength()-6);
    }
    else
    if (Stuff.Left(9) == "Position:")
    {
      Position = Stuff.Right(Stuff.GetLength()-9);
    }
    ReadLine();
  }
  CloseFile();
}

CString Player::GetOutput()
{
  return Output;
}

/***********************************************************
* New player??                                             *
************************************************************/

bool Player::IsNew(CString InpName)
{
  CString     FileName;
  CStdioFile  PlayerFile;
  int         Success;
  
  FileName = ".\\Players\\" + InpName + ".txt";
  Success = PlayerFile.Open(FileName,
            CFile::modeRead |
            CFile::typeText);
  if(Success)
  {
    return false;
    PlayerFile.Close();
  }
  else
  {
    return true;
  }
}

/***********************************************************
* Show player status                                       *
************************************************************/

void Player::ShowStatus()
{
  CString TmpStr;

  Output = "\r\n";

  Output += "Name:       ";
  Output += Name;
  Output += "\r\n";

  TmpStr.Format("%d", Level);
  Output += "Level:      " + TmpStr + "\r\n";

  TmpStr.Format("%d", Experience);
  Output += "Experience: " + TmpStr + "\r\n";

  Output += "Position:   ";
  TmpStr = Position;
  TmpStr.Replace("s", "S");
  if (TmpStr == "Sit")
  { // Add extra 't'
    TmpStr += "t";
  }
  TmpStr += "ing";
  Output += TmpStr;
  Output += "\r\n";

  Output += "Sex:        ";
  Output += Sex;
  Output += "\r\n";

  TmpStr.Format("%d", Gold);
  Output += "Gold:       " + TmpStr + "\r\n";
  
  TmpStr.Format("%d", Silver);
  Output += "Silver:     " + TmpStr + "\r\n";

  TmpStr.Format("%d", Bronze);
  Output += "Bronze:     " + TmpStr + "\r\n";

  TmpStr.Format("%d", Copper);
  Output += "Copper:     " + TmpStr + "\r\n";
}

/***********************************************************
* Create player prompt                                     *
************************************************************/

void Player::CreatePrompt()
{
  CString TmpStr;

  Output = "";
  Output += "\r\n";
  TmpStr.Format("%d", HitPoints);
  Output += TmpStr + "H ";
  TmpStr.Format("%d", MovePoints);
  Output += TmpStr + "M ";
  Output += "> ";
}