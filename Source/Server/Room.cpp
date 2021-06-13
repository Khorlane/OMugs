/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Room.cpp                                         *
* Usage:  Manages rooms                                    *
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
#include "Room.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern Dnode   *pDnodeActor;
extern Dnode   *pDnodeSrc;
extern Dnode   *pDnodeTgt;
extern sqlite3 *pWorldDb;
extern CString  CmdStr;
extern CString  MudCmd;
extern CString  SqlStatement;

sqlite3_stmt *pStmt;
CStdioFile    RoomFile;
bool          SqlNotFound;
int           SqlResult;

/***********************************************************
* Room constructor                                         *
************************************************************/

Room::Room()
{
}

/***********************************************************
* Room destructor                                          *
************************************************************/

Room::~Room()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Get RoomId                                               *
************************************************************/

CString Room::GetRoomId(CString RoomId)
{
  CStdioFile RoomFile;
  CString    RoomFileName;
  CString    Stuff;
  int        Success;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  Success = RoomFile.Open(RoomFileName,
               CFile::modeRead |
               CFile::typeText);
  if(!Success)
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::GetRoomId - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  // RoomId
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(7) != "RoomId:")
  { // Very bad, where did the RoomId go anyway?
    AfxMessageBox("Room::GetRoomId - RoomId: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomId = Utility::GetWord(Stuff, 2);
  RoomFile.Close();
  return RoomId;
}

/***********************************************************
* Get RoomName                                             *
************************************************************/

CString Room::GetRoomName(CString RoomId)
{
  CStdioFile RoomFile;
  CString    RoomFileName;
  CString    RoomName;
  CString    Stuff;
  int        Success;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  Success = RoomFile.Open(RoomFileName,
               CFile::modeRead |
               CFile::typeText);
  if(!Success)
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::GetRoomName - Room does not exist", MB_ICONSTOP);
    _endthread();
  }

  // RoomName
  RoomFile.ReadString(Stuff);
  RoomFile.ReadString(Stuff);
  RoomFile.ReadString(Stuff);
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(9) != "RoomName:")
  { // Very bad, where did the RoomName go anyway?
    AfxMessageBox("Room::GetRoomName - RoomName: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomName = Utility::GetWords(Stuff, 2);
  RoomName.TrimLeft();
  RoomName.TrimRight();
  RoomFile.Close();
  return RoomName;
}

/***********************************************************
* Get the list of exits that mobiles are allowed to use    *
************************************************************/

CString Room::GetValidMobRoomExits(CString RoomId)
{
  CString    ExitToRoomId;
  CStdioFile RoomFile;
  CString    RoomFileName;
  CString    Stuff;
  int        Success;
  CString    ValidMobExits;

  RoomFileName = ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  Success = RoomFile.Open(RoomFileName,
    CFile::modeRead |
    CFile::typeText);
  if (!Success)
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::GetValidMobRoomExits - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  ValidMobExits = "";
  Stuff = "Not Done";
  while (Stuff != "End of Exits")
  { // Loop - process all exits
    RoomFile.ReadString(Stuff);
    if (Stuff.Left(13) == "ExitToRoomId:")
    { // An Exit has been found
      ExitToRoomId = Utility::GetWord(Stuff, 2);
      if (ExitToRoomId == "VineyardPath382")
        Success = 100;
      if (!Room::IsRoomType(ExitToRoomId, "NoNPC"))
      { // And it's a valid Mob Exit
        ValidMobExits += ExitToRoomId;
        ValidMobExits += " ";
      }
    }
  }
  ValidMobExits.TrimRight();
  RoomFile.Close();
  return ValidMobExits;
}

/***********************************************************
* If valid room exit, then deal with it                    *
************************************************************/

bool Room::IsExit(CString MudCmdIsExit)
{
  bool     Found;
  CString  ExitLookup;
  CString  ExitName;
  CString  ExitToRoomId;
  CString  Stuff;
  CString  TmpStr;

  Found = false;
  if (!OpenFile(pDnodeActor))
  { // If the file isn't there, then the Room does not exit, doh!
    AfxMessageBox("Room::IsExit - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  ExitLookup = Utility::GetWord(CmdStr, 2);
  ExitLookup.MakeLower();
  ExitLookup = Utility::TranslateWord(ExitLookup);
  Stuff = "Not Done";
  while (Stuff != "End of Exits")
  { // Loop until Exit is found or end of file
    RoomFile.ReadString(Stuff);
    if (Stuff.Left(9) == "ExitName:")
    { // Ok, an Exit has been found
      ExitName = Utility::GetWord(Stuff, 2);
      ExitName.MakeLower();
      ExitName = Utility::TranslateWord(ExitName);
      if (ExitName == ExitLookup)
      { // THE Exit has been found
        Found = true;
        Stuff = "End of Exits";
      }
    }
  }
  if (Found)
  { // At this point we know that the command entered referred to a valid exit
    if (Communication::IsSleeping())
    { // Player is sleeping, send msg, command is not done
      CloseFile();
      return true;
    }
    if (pDnodeActor->pPlayer->Position == "sit")
    { // The player is sitting, abort the move
      CloseFile();
      pDnodeActor->PlayerOut += "You must be standing before you can move.";
      pDnodeActor->PlayerOut += "\r\n";
      pDnodeActor->pPlayer->CreatePrompt();
      pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
      return true;
    }
    if (MudCmdIsExit == "go")
    { // Go command
      if (pDnodeActor->pPlayer->pPlayerFollowers[0] != NULL)
      { // If player is following another player
        if (MudCmd != "flee")
        { // And not fleeing, abort the move
          CloseFile();
          pDnodeActor->PlayerOut += "Can't honor your command, you are following ";
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->pPlayerFollowers[0]->Name;
          pDnodeActor->PlayerOut += ".\r\n";
          pDnodeActor->pPlayer->CreatePrompt();
          pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
          return true;
        }
      }
      while (Stuff.Left(13) != "ExitToRoomId:")
      { // Position to ExitToRoomId line
        RoomFile.ReadString(Stuff);
      }
      ExitToRoomId = Utility::GetWord(Stuff, 2);
      MovePlayer(pDnodeActor, ExitToRoomId);
      CloseFile();
      ShowRoom(pDnodeActor);
      if (pDnodeActor->pPlayer->PlayerRoomHasNotBeenHere())
      { // Player has not been here (on their own), give some experience
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->PlayerOut += "&YYou gain experience by exploring!&N";
        pDnodeActor->PlayerOut += "\r\n";
        pDnodeActor->pPlayer->CreatePrompt();
        pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
        pDnodeActor->pPlayer->GainExperience(pDnodeActor, 25);
      }
      if (MudCmd != "flee")
      { // Player is not fleeing
        MoveFollowers(pDnodeActor, ExitToRoomId);
      }
    }
    else
    { // MudCmd was not 'go'
      if (MudCmdIsExit == "look")
      { // Look at an exit
        ShowRoomExitDesc();
        CloseFile();
      }
    }
    return true; // so command processor will exit properly
  }
  else
  { // At this point we know that the command entered did NOT referred to a valid exit
    CloseFile(); 
    return false; // so command processor will keep trying
  }
}

/***********************************************************
* Is this a valid room?                                    *
************************************************************/

bool Room::IsRoom(CString RoomId)
{
  CStdioFile RoomFile;
  CString    RoomFileName;
  int        Success;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  Success = RoomFile.Open(RoomFileName,
               CFile::modeRead |
               CFile::typeText);
  if(Success)
  {
    RoomFile.Close();
    return true;
  }
  else
  {
    return false;
  }
}

bool Room::IsRoomType(CString RoomId, CString RoomType)
{
  CStdioFile RoomFile;
  CString    RoomFileName;
  CString    Stuff;
  int        Success;

  RoomFileName = ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  Success = RoomFile.Open(RoomFileName,
    CFile::modeRead |
    CFile::typeText);
  if (!Success)
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::IsRoomType - Room does not exist", MB_ICONSTOP);
    _endthread();
  }

  // RoomType
  RoomFile.ReadString(Stuff);
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(9) != "RoomType:")
  { // Very bad, where did the RoomType go anyway?
    AfxMessageBox("Room::IsRoomType - RoomType: not found", MB_ICONSTOP);
    _endthread();
  }
  Stuff = Utility::GetWords(Stuff, 2);
  Stuff.TrimLeft();
  Stuff.TrimRight();
  RoomFile.Close();
  if (Stuff != RoomType)
  { // No matching RoomType found
    return false;
  }
  else
  { // Matching RoomType found
    return true;
  }
}

/***********************************************************
* Show the room to the player                              *
************************************************************/

void Room::ShowRoom(Dnode *pDnode)
{
  if (!OpenFile(pDnode))
  { // If the file isn't there, then the Room does not exit, doh!
    AfxMessageBox("Room::ShowRoom - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  ShowRoomName(pDnode);
  ShowRoomDesc(pDnode);
  ShowRoomExits(pDnode);
  CloseFile();
  Communication::ShowPlayersInRoom(pDnode);
  Object::ShowObjsInRoom(pDnode);
  Mobile::ShowMobsInRoom(pDnode);
  pDnode->PlayerOut += "\r\n";
  pDnode->pPlayer->CreatePrompt();
  pDnode->PlayerOut += pDnode->pPlayer->GetOutput();
}

////////////////////////////////////////////////////////////
// Private functions static                               //
////////////////////////////////////////////////////////////

/***********************************************************
* Close Room file                                          *
************************************************************/

void Room::CloseFile()
{
  RoomFile.Close();
}

/***********************************************************
* Move followers                                           *
************************************************************/

void Room::MoveFollowers(Dnode *pDnode, CString ExitToRoomId)
{ // Recursive
  Dnode *pDnodeGrpMem;
  int    i;

  for (i = 1; i < GRP_LIMIT; i++)
  {
    if (!pDnode->pPlayer->pPlayerFollowers[i])
    { // No followers or no more followers
     return;
    }
    pDnodeGrpMem = Communication::GetTargetDnode(pDnode->pPlayer->pPlayerFollowers[i]->Name);
    if (!pDnodeGrpMem)
    { // Follower is not online and/or not in 'playing' state
      continue;
    }
    if (pDnode->pPlayer->RoomIdBeforeMove != pDnodeGrpMem->pPlayer->RoomId)
    { // Not in same room, can't follow
      continue;
    }
    MovePlayer(pDnodeGrpMem, ExitToRoomId);
    ShowRoom(pDnodeGrpMem);
    MoveFollowers(pDnodeGrpMem, ExitToRoomId);
  }
}

/***********************************************************
* Go command - move the player                             *
************************************************************/

void Room::MovePlayer(Dnode *pDnode, CString ExitToRoomId)
{
  CString TmpStr;
  CString MoveMsg;

  pDnodeSrc = pDnode;
  pDnodeTgt = pDnode;
  // Leaves message
  if (MudCmd != "flee")
  { // If player is not fleeing
    MoveMsg = pDnode->PlayerName + " leaves.";
    Communication::SendToRoom(pDnode->pPlayer->RoomId, MoveMsg);
  }
  // Switch rooms
  pDnode->pPlayer->RoomIdBeforeMove = pDnode->pPlayer->RoomId;
  pDnode->pPlayer->RoomId = ExitToRoomId;
  World::Osi("Rooms", ExitToRoomId);
  pDnode->pPlayer->Save();
  // Arrives message
  MoveMsg = pDnode->PlayerName + " arrives.";
  Communication::SendToRoom(pDnode->pPlayer->RoomId, MoveMsg);
}

/***********************************************************
* Open Room file                                           *
************************************************************/

bool Room::OpenFile(Dnode *pDnode)
{
  CString RoomFileName;
  int     Success;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += pDnode->pPlayer->RoomId;
  RoomFileName += ".txt";
  Success = RoomFile.Open(RoomFileName,
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
* Show the room description to the player                  *
************************************************************/

void Room::ShowRoomDesc(Dnode *pDnode)
{
  CString Stuff;

  // RoomDesc
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(9) != "RoomDesc:")
  {
    AfxMessageBox("Room::ShowRoomDesc - RoomDesc: not found", MB_ICONSTOP);
    _endthread();
  }
  // Room Description
  RoomFile.ReadString(Stuff);
  while (Stuff != "End of RoomDesc")
  {
    pDnode->PlayerOut += Stuff;
    pDnode->PlayerOut += "\r\n";
    RoomFile.ReadString(Stuff);
  }
}

/***********************************************************
* Show exit description to player                          *
************************************************************/

void Room::ShowRoomExitDesc()
{
  CString Stuff;

  // ExitDesc
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(9) != "ExitDesc:")
  {
    AfxMessageBox("Room::ShowRoomExitDesc - ExitDesc: not found", MB_ICONSTOP);
    _endthread();
  }
  // Exit Description
  RoomFile.ReadString(Stuff);
  while (Stuff.Left(13) != "ExitToRoomId:")
  {
    pDnodeActor->PlayerOut += Stuff;
    pDnodeActor->PlayerOut += "\r\n";
    RoomFile.ReadString(Stuff);
  }
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}

/***********************************************************
* Show the room exits to the player                        *
************************************************************/

void Room::ShowRoomExits(Dnode *pDnode)
{
  bool    NoExits;
  CString Stuff;

  NoExits = true;
  pDnode->PlayerOut += "&C";
  pDnode->PlayerOut += "Exits:";
  while (Stuff != "End of Exits")
  {
    RoomFile.ReadString(Stuff);
    if (Stuff.Left(9) == "ExitName:")
    {
      NoExits = false;
      Stuff = Utility::GetWord(Stuff, 2);
      pDnode->PlayerOut += " ";
      pDnode->PlayerOut += Stuff;
    }
  }
  if (NoExits)
  {
    pDnode->PlayerOut += " None";
  }
  pDnode->PlayerOut += "&N";
}

/***********************************************************
* Show the room name to the player                         *
************************************************************/

void Room::ShowRoomName(Dnode *pDnode)
{
  CString RoomId;
  CString RoomType;
  CString Stuff;
  CString Terrain;

  // RoomId
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(7) != "RoomId:")
  {
    AfxMessageBox("Room::ShowRoomName - RoomId: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomId = Utility::GetWord(Stuff, 2);
  if (RoomId != pDnode->pPlayer->RoomId)
  {
    AfxMessageBox("Room::ShowRoomName - RoomId mis-match", MB_ICONSTOP);
    _endthread();
  }
  // RoomType
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(9) != "RoomType:")
  {
    AfxMessageBox("Room::ShowRoomName - RoomType: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomType = Utility::GetWords(Stuff, 2);
  // Terrain
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(8) != "Terrain:")
  {
    AfxMessageBox("Room::ShowRoomName - Terrain: not found", MB_ICONSTOP);
    _endthread();
  }
  Terrain = Utility::GetWord(Stuff, 2);
  // RoomName
  RoomFile.ReadString(Stuff);
  if (Stuff.Left(9) != "RoomName:")
  {
    AfxMessageBox("Room::ShowRoomName - RoomName: not found", MB_ICONSTOP);
    _endthread();
  }
  Stuff = Utility::GetWords(Stuff, 2);
  Stuff.TrimLeft();
  pDnode->PlayerOut += "\r\n";
  pDnode->PlayerOut += "&C";
  pDnode->PlayerOut += Stuff;
  pDnode->PlayerOut += "&N";
  if (pDnode->pPlayer->RoomInfo)
  { // Show hidden room info
    pDnode->PlayerOut += "&M";
    pDnode->PlayerOut += " [";
    pDnode->PlayerOut += "&N";
    pDnode->PlayerOut += RoomId;
    pDnode->PlayerOut += " ";
    pDnode->PlayerOut += Terrain;
    pDnode->PlayerOut += " ";
    pDnode->PlayerOut += RoomType;
    pDnode->PlayerOut += "&M";
    pDnode->PlayerOut += "]";
    pDnode->PlayerOut += "&N";
  }
  pDnode->PlayerOut += "\r\n";
}