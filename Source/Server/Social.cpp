/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Social.cpp                                       *
* Usage:  Displays social messages to players              *
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

#include "stdafx.h"         // precompiled headers
#include "Social.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern Dnode   *pDnodeActor;
extern Dnode   *pDnodeSrc;
extern Dnode   *pDnodeTgt;
extern CString  CmdStr;
extern CString  MudCmd;

CStdioFile SocialFile;

/***********************************************************
* Social constructor                                       *
************************************************************/

Social::Social()
{
}

/***********************************************************
* Socail destructor                                        *
************************************************************/

Social::~Social()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Is MudCmd a social command?                              *
************************************************************/

bool Social::IsSocial()
{
  bool     Found;
  CString  MinPos;
  CString  MsgText;
  CString  TmpStr;

  Found = false;
  if (!OpenFile())
  { // If the file isn't, then all socials are bad commands, doh!
    return false;
  }
  MsgText = "Not Done";
  while (MsgText != "End of Socials")
  { // Loop until social is found or end of file
    MsgText = ReadLine();
    MsgText.TrimLeft();
    MsgText.TrimLeft();
    if (MsgText.Left(9) == "Social : ")
    { // Ok, a social has been found
      TmpStr = MsgText.Right(MsgText.GetLength() - 9);
      if (TmpStr == MudCmd)
      { // THE social has been found
        Found = true;
        MsgText = ReadLine();
        MinPos = MsgText.Right(MsgText.GetLength() - 9);
        if (PositionNotOk(pDnodeActor, MinPos))
        { // Player is not in the minimum position
          pDnodeActor->PlayerOut += "You are not in a position to that right now.\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          MsgText = "";
          break;
        }
        // It is a social and player is in minimum position
        Socialize(MinPos, MsgText);
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

////////////////////////////////////////////////////////////
// Private functions static                               //
////////////////////////////////////////////////////////////

/***********************************************************
* Close social file                                        *
************************************************************/

void Social::CloseFile()
{
  SocialFile.Close();
}

/***********************************************************
* Open social file                                         *
************************************************************/

bool Social::OpenFile()
{
  CString SocialFileName;
  int     Success;
  
  SocialFileName =  SOCIAL_DIR;
  SocialFileName += "Social.txt";
  Success = SocialFile.Open(SocialFileName,
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
* Is player in a valid position for the social             *
************************************************************/

bool Social::PositionNotOk(Dnode *pDnode, CString MinPos)
{
  int MinPosNbr;
  int PlayerPosNbr;

  MinPosNbr = PosNbr(MinPos);
  PlayerPosNbr = PosNbr(pDnodeActor->pPlayer->Position);
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
* Read a line from social file                             *
************************************************************/

CString Social::ReadLine()
{
  CString MsgText;

  SocialFile.ReadString(MsgText);
  return MsgText;
}

/***********************************************************
* Send substituted message to player                       *
************************************************************/

void Social::SendToPlayer(CString MsgText)
{
  pDnodeActor->PlayerOut += MsgText;
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Send substituted message to target                       *
************************************************************/

void Social::SendToTarget(Dnode *pDnodeTgt, CString MsgText)
{
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->PlayerOut += MsgText;
  pDnodeTgt->PlayerOut += "\r\n";
  pDnodeTgt->pPlayer->CreatePrompt();
  pDnodeTgt->PlayerOut += pDnodeTgt->pPlayer->GetOutput();
}

/***********************************************************
* Social command                                           *
************************************************************/

void Social::Socialize(CString MinPos, CString MsgText)
{
  int      i;
  int      LineCount;
  CString  PlayerName;
  CString  TargetName;
  CString  TargetNameSave;
  bool     TargetNotHere;

  pDnodeSrc = pDnodeActor;
  pDnodeTgt = NULL;
  TargetNotHere = false;
  MsgText = ReadLine();
  LineCount = atoi(MsgText.Right(MsgText.GetLength() - 9));
  PlayerName = pDnodeActor->PlayerName;
  PlayerName.MakeLower();
  TargetName = Utility::GetWord(CmdStr, 2);
  TargetNameSave = TargetName;
  TargetName.MakeLower();
  if (LineCount == 2)
  { // The social does not accept a target
    if (TargetName != "")
    { // But, target was given
      pDnodeSrc->PlayerOut += MudCmd;
      pDnodeSrc->PlayerOut += " does not use a target.\r\n";
      pDnodeSrc->pPlayer->CreatePrompt();
      pDnodeSrc->PlayerOut += pDnodeSrc->pPlayer->GetOutput();      
      return;
    }
  }
  // All checks complete, get on with the social
  if (TargetName == "")
  { // Social without target
    MsgText = ReadLine();
    MsgText = Utility::PronounSubstitute(MsgText);
    SendToPlayer(MsgText);
    MsgText = ReadLine();
    MsgText = Utility::PronounSubstitute(MsgText);
    Communication::SendToRoom(pDnodeActor->pPlayer->RoomId, MsgText);
    return;
  }
  if (PlayerName == TargetName)
  { // Social with target equal self
    for (i = 1; i <= 3; i++)
    {
      MsgText = ReadLine();
    }
    MsgText = Utility::PronounSubstitute(MsgText);
    SendToPlayer(MsgText);
    return;
  }
  // Do some checks to determine if target is valid
  pDnodeTgt = Communication::GetTargetDnode(TargetName);
  if (!pDnodeTgt)
  { // Target is not online and/or not in 'playing' state
    TargetNotHere = true;
  }
  else
  { // Target is online and playing
    if (pDnodeSrc->pPlayer->RoomId != pDnodeTgt->pPlayer->RoomId)
    { // Target is not in the same room
      TargetNotHere = true;
    }
  }
  if (TargetNotHere)
  { // Target is not playing or is not in same room as player
    for (i = 1; i <= 4; i++)
    {
      MsgText = ReadLine();
    }
    MsgText = Utility::PronounSubstitute(MsgText);
    SendToPlayer(MsgText);
    return;
  }
  else
  { // Target is playing and is in same room as player
    if (PositionNotOk(pDnodeTgt, MinPos))
    { // Target is not in minimum position for social
      pDnodeSrc->PlayerOut += pDnodeTgt->PlayerName;
      pDnodeSrc->PlayerOut += " is ";
      pDnodeSrc->PlayerOut += pDnodeTgt->pPlayer->Position;
      if (pDnodeTgt->pPlayer->Position == "sit")
      { // Add the extra 't' so it comes out sitting vs siting
        pDnodeSrc->PlayerOut += "t";
      }
      pDnodeSrc->PlayerOut += "ing and cannot participate.\r\n";
      pDnodeSrc->pPlayer->CreatePrompt();
      pDnodeSrc->PlayerOut += pDnodeSrc->pPlayer->GetOutput();      
      return;
    }
    for (i = 1; i <= 5; i++)
    {
      MsgText = ReadLine();
    }
    // Message to player
    MsgText = Utility::PronounSubstitute(MsgText);
    SendToPlayer(MsgText);
    MsgText = ReadLine();
    // Message to target
    MsgText = Utility::PronounSubstitute(MsgText);
    SendToTarget(pDnodeTgt, MsgText);
    MsgText = ReadLine();
    // Message to the others
    MsgText = Utility::PronounSubstitute(MsgText);
    Communication::SendToRoom(pDnodeActor->pPlayer->RoomId, MsgText);
  }
}