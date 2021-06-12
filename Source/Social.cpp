/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Social.cpp                                       *
* Usage:  Handles all social commands                      *
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
#include "Communication.h"
#include "Player.h"
#include "Social.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern CString        GetWord(CString CmdStr, int WordNbr);
extern CString        GetWords(CString CmdStr, int WordNbr);
extern Communication  *pComm;

/***********************************************************
* Log constructor                                          *
************************************************************/

Social::Social()
{
}

/***********************************************************
* Log destructor                                           *
************************************************************/

Social::~Social()
{
}

/***********************************************************
* Is MudCmd a social command?                              *
************************************************************/

bool Social::IsSocial(Dnode *pDnode1, CString CmdStr, CString MudCmd)
{
  bool    Found;
  CString TmpStr;

  Found = false;
  if (!OpenFile())
  { // If the file isn't, then all socials are bad commands, doh!
    return false;
  }
  MsgText = "Not Done";
  while (MsgText != "")
  { // Loop until social is found or end of file
    ReadLine();
    MsgText.TrimLeft();
    MsgText.TrimLeft();
    if (MsgText.Left(9) == "Social : ")
    { // Ok, a social has been found
      TmpStr = MsgText.Right(MsgText.GetLength() - 9);
      if (TmpStr == MudCmd)
      { // THE social has been found
        Found = true;
        ReadLine();
        MinPos = MsgText.Right(MsgText.GetLength() - 9);
        if (PositionNotOk(pDnode1))
        { // Player is not in the minimum position
          pDnode1->PlayerOut += "You are not in a position to that right now.\r\n";
          pDnode1->pPlayer->CreatePrompt();
          pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
          MsgText = "";
          break;
        }
        // It is a social and player is in minimum position
        Socialize(pDnode1, CmdStr, MudCmd);
      }
    }
  }
  CloseFile();
  if (Found)
  { // Return true so command processor will exit properly
    return true;
  }
  else
  { // Return false so command processor will tell player bad command
    return false;
  }
}

/***********************************************************
* Open social file                                         *
************************************************************/

bool Social::OpenFile()
{
  CString     FileName;
  int         Success;
  
  FileName =  ".\\Library\\";
  FileName += "Social.txt";
  Success = SocialFile.Open(FileName,
            CFile::modeRead |
            CFile::typeText);
  if(Success)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/***********************************************************
* Close social file                                        *
************************************************************/

void Social::CloseFile()
{
  SocialFile.Close();
}

/***********************************************************
* Read a line from social file                             *
************************************************************/

void Social::ReadLine()
{
  SocialFile.ReadString(MsgText);
}

/***********************************************************
* Convert position to a number                             *
************************************************************/

int Social::PosNbr(CString Position)
{
  if (Position == "sleep")
  {
    return 1;
  }
  if (Position == "sit")
  {
    return 2;
  }
  if (Position == "stand")
  {
    return 3;
  }
  return -1;
}

/***********************************************************
* Is player in a valid position for the social             *
************************************************************/

bool Social::PositionNotOk(Dnode *pDnode1)
{
  int         MinPosNbr;
  int         PlayerPosNbr;

  MinPosNbr = PosNbr(MinPos);
  PlayerPosNbr = PosNbr(pDnode1->pPlayer->Position);
  if (PlayerPosNbr < MinPosNbr)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/***********************************************************
* Social command                                           *
************************************************************/

void Social::Socialize(Dnode *pDnode1, CString CmdStr, CString MudCmd)
{
  Dnode   *pDnode2;
  int     i;
  int     LineCount;
  CString PlayerName;
  CString TargetName;
  CString TargetNameSave;
  bool    TargetNotHere;

  pDnode2 = NULL;
  TargetNotHere = false;
  ReadLine();
  LineCount = atoi(MsgText.Right(MsgText.GetLength() - 9));
  PlayerName = pDnode1->PlayerName;
  PlayerName.MakeLower();
  TargetName = GetWord(CmdStr, 2);
  TargetNameSave = TargetName;
  TargetName.MakeLower();
  if (LineCount == 2)
  { // The social does not accept a target
    if (TargetName != "")
    { // But, target was given
      pDnode1->PlayerOut += MudCmd;
      pDnode1->PlayerOut += " does not use a target.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();      
      return;
    }
  }
  // All checks complete, get on with the social
  if (TargetName == "")
  { // Social without target
    ReadLine();
    Substitute(pDnode1, pDnode2);
    SendToPlayer(pDnode1);
    ReadLine();
    Substitute(pDnode1, pDnode2);
    pComm->SendToRoom(pDnode1, pDnode2, MsgText);
    return;
  }
  if (PlayerName == TargetName)
  { // Social with target equal self
    for (i=1; i<=3; i++)
    {
      ReadLine();
    }
    Substitute(pDnode1, pDnode2);
    SendToPlayer(pDnode1);
    return;
  }
  // Do some checks to determine if target is valid
  pDnode2 = pComm->GetTargetDnode(TargetName);
  if (!pDnode2)
  { // Target is not online and/or not in 'playing' state
    TargetNotHere = true;
  }
  else
  { // Target is online and playing
    if (pDnode1->pPlayer->RoomId != pDnode2->pPlayer->RoomId)
    { // Target is not in the same room
      TargetNotHere = true;
    }
  }
  if (TargetNotHere)
  { // Target is not playing or is not in same room as player
    for (i=1; i<=4; i++)
    {
      ReadLine();
    }
    Substitute(pDnode1, pDnode2);
    SendToPlayer(pDnode1);
    return;
  }
  else
  { // Target is playing and is in same room as player
    if (PositionNotOk(pDnode2))
    { // Target is not in minimum position for social
      pDnode1->PlayerOut += pDnode2->PlayerName;
      pDnode1->PlayerOut += " is ";
      pDnode1->PlayerOut += pDnode2->pPlayer->Position;
      if (pDnode2->pPlayer->Position == "sit")
      { // Add the extra 't' so it comes out sitting vs siting
        pDnode1->PlayerOut += "t";
      }
      pDnode1->PlayerOut += "ing and cannot participate.\r\n";
      pDnode1->pPlayer->CreatePrompt();
      pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();      
      return;
    }
    for (i=1; i<=5; i++)
    {
      ReadLine();
    }
    Substitute(pDnode1, pDnode2);
    SendToPlayer(pDnode1);
    ReadLine();
    Substitute(pDnode1, pDnode2);
    SendToTarget(pDnode2);
    ReadLine();
    Substitute(pDnode1, pDnode2);
    pComm->SendToRoom(pDnode1, pDnode2, MsgText);
  }
}

/***********************************************************
* Substitute $thingies to something more meaningful        *
************************************************************/

void Social::Substitute(Dnode *pDnode1, Dnode *pDnode2)
{
  CString PronounHeShe;
  CString PronounHimHer;
  CString PronounHisHers;

  if (pDnode1->pPlayer->Sex == "M")
  { // Set male player pronouns
    PronounHeShe    = "he";
    PronounHimHer   = "him";
    PronounHisHers  = "his";
  }
  else
  { // Set female player pronouns
    PronounHeShe    = "she";
    PronounHimHer   = "her";
    PronounHisHers  = "hers";
  }
  // Make the player substitutions
  MsgText.Replace("$P",         pDnode1->PlayerName);
  MsgText.Replace("$pHeShe",    PronounHeShe);
  MsgText.Replace("$pHimHer",   PronounHimHer);
  MsgText.Replace("$pHisHers",  PronounHisHers);
  if (pDnode2)
  { // There is a valid target
    if (pDnode2->pPlayer->Sex == "M")
    { // Set male target pronouns
      PronounHeShe    = "he";
      PronounHimHer   = "him";
      PronounHisHers  = "his";
    }
    else
    { // Set female target pronouns
      PronounHeShe    = "she";
      PronounHimHer   = "her";
      PronounHisHers  = "hers";
    }
    // Make the target substitutions
    MsgText.Replace("$T",         pDnode2->PlayerName);
    MsgText.Replace("$tHeShe",    PronounHeShe);
    MsgText.Replace("$tHimHer",   PronounHimHer);
    MsgText.Replace("$tHisHers",  PronounHisHers);
  }
}

/***********************************************************
* Send substituted message to player                       *
************************************************************/

void Social::SendToPlayer(Dnode *pDnode1)
{
  pDnode1->PlayerOut += MsgText;
  pDnode1->PlayerOut += "\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/***********************************************************
* Send substituted message to target                       *
************************************************************/

void Social::SendToTarget(Dnode *pDnode2)
{
  pDnode2->PlayerOut += "\r\n";
  pDnode2->PlayerOut += MsgText;
  pDnode2->PlayerOut += "\r\n";
  pDnode2->pPlayer->CreatePrompt();
  pDnode2->PlayerOut += pDnode2->pPlayer->GetOutput();
}