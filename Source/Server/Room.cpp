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

#include "stdafx.h" // This is only here to speed up compiles
#include "Room.h"

/***********************************************************
* Globals                                                  *
************************************************************/

ifstream      RoomFile;

Dnode *GetTargetDnode( string TargetName); // Communication
void   ShowMobsInRoom(Dnode *pDnode);      // Mobile
void   ShowObjsInRoom(Dnode *pDnode);      // Object
void   ShowPlayersInRoom(Dnode *pDnode);   // Communication

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

string Room::GetRoomId(string RoomId)
{
  string    RoomFileName;
  string    Stuff;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  RoomFile.open(RoomFileName);
  if(!RoomFile.is_open())
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::GetRoomId - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  // RoomId
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 7) != "RoomId:")
  { // Very bad, where did the RoomId go anyway?
    AfxMessageBox("Room::GetRoomId - RoomId: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomId = StrGetWord(Stuff, 2);
  RoomFile.close();
  return RoomId;
}

/***********************************************************
* Get RoomName                                             *
************************************************************/

string Room::GetRoomName(string RoomId)
{
  string     RoomFileName;
  string     RoomName;
  string     Stuff;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  RoomFile.open(RoomFileName);
  if (!RoomFile.is_open())
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::GetRoomName - Room does not exist", MB_ICONSTOP);
    _endthread();
  }

  // RoomName
  getline(RoomFile, Stuff);
  getline(RoomFile, Stuff);
  getline(RoomFile, Stuff);
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 9) != "RoomName:")
  { // Very bad, where did the RoomName go anyway?
    AfxMessageBox("Room::GetRoomName - RoomName: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomName = StrGetWords(Stuff, 2);
  StrTrimLeft(RoomName);
  StrTrimRight(RoomName);
  RoomFile.close();
  return RoomName;
}

/***********************************************************
* Get the list of exits that mobiles are allowed to use    *
************************************************************/

string Room::GetValidMobRoomExits(string RoomId)
{
  string    ExitToRoomId;
  string    RoomFileName;
  string    Stuff;
  string    ValidMobExits;

  RoomFileName = ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  RoomFile.open(RoomFileName);
  if (!RoomFile.is_open())
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::GetValidMobRoomExits - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  ValidMobExits = "";
  Stuff = "Not Done";
  while (Stuff != "End of Exits")
  { // Loop - process all exits
    getline(RoomFile, Stuff);
    if (StrLeft(Stuff, 13) == "ExitToRoomId:")
    { // An Exit has been found
      ExitToRoomId = StrGetWord(Stuff, 2);
      if (ExitToRoomId == "VineyardPath382")
        //Success = 100;
        int x = 0;
      if (!IsRoomType(ExitToRoomId, "NoNPC"))
      { // And it's a valid Mob Exit
        ValidMobExits += ExitToRoomId;
        ValidMobExits += " ";
      }
    }
  }
  StrTrimRight(ValidMobExits);
  RoomFile.close();
  return ValidMobExits;
}

/***********************************************************
* If valid room exit, then deal with it                    *
************************************************************/

bool Room::IsExit(string MudCmdIsExit)
{
  bool     Found;
  string   ExitLookup;
  string   ExitName;
  string   ExitToRoomId;
  string   Stuff;
  string   TmpStr;

  //string  sCmdStr;
  //sCmdStr = ConvertCStringToString(CmdStr);

  Found = false;
  if (!OpenFile(pDnodeActor))
  { // If the file isn't there, then the Room does not exit, doh!
    AfxMessageBox("Room::IsExit - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  ExitLookup = StrGetWord(CmdStr, 2);
  ExitLookup = StrMakeLower(ExitLookup);
  ExitLookup = TranslateWord(ConvertStringToCString(ExitLookup));
  Stuff = "Not Done";
  while (Stuff != "End of Exits")
  { // Loop until Exit is found or end of file
    getline(RoomFile, Stuff);
    if (StrLeft(Stuff, 9) == "ExitName:")
    { // Ok, an Exit has been found
      ExitName = StrGetWord(Stuff, 2);
      ExitName = StrMakeLower(ExitName);
      ExitName = TranslateWord(ConvertStringToCString(ExitName));
      if (ExitName == ExitLookup)
      { // THE Exit has been found
        Found = true;
        Stuff = "End of Exits";
      }
    }
  }
  if (Found)
  { // At this point we know that the command entered referred to a valid exit
    if (IsSleeping())
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
      while (StrLeft(Stuff, 13) != "ExitToRoomId:")
      { // Position to ExitToRoomId line
        getline(RoomFile, Stuff);
      }
      ExitToRoomId = StrGetWord(Stuff, 2);
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

bool Room::IsRoom(string RoomId)
{
  string     RoomFileName;
  string     Stuff;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  RoomFile.open(RoomFileName);
  if (RoomFile.is_open())
  {
    getline(RoomFile, Stuff);
    RoomFile.close();
    if (StrLeft(Stuff, 7) != "RoomId:")
    {
      AfxMessageBox("Room::IsRoom - RoomId: not found", MB_ICONSTOP);
      _endthread();
    }
    Stuff = StrGetWord(Stuff, 2);
    if (Stuff != RoomId)
    {
      return false;
    }
    return true;
  }
  else
  {
    return false;
  }
}

bool Room::IsRoomType(string RoomId, string RoomType)
{
  ifstream   RoomFile;
  string     RoomFileName;
  string     Stuff;

  RoomFileName = ROOMS_DIR;
  RoomFileName += RoomId;
  RoomFileName += ".txt";
  RoomFile.open(RoomFileName);
  if (!RoomFile.is_open())
  { // No such file???, But there should be, This is bad!
    AfxMessageBox("Room::IsRoomType - Room does not exist", MB_ICONSTOP);
    _endthread();
  }
  // RoomType
  getline(RoomFile, Stuff);
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 9) != "RoomType:")
  { // Very bad, where did the RoomType go anyway?
    AfxMessageBox("Room::IsRoomType - RoomType: not found", MB_ICONSTOP);
    _endthread();
  }
  Stuff = StrGetWords(Stuff, 2);
  StrTrimLeft(Stuff);
  StrTrimRight(Stuff);
  RoomFile.close();
  if (StrIsNotWord(RoomType, Stuff))
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
  ShowPlayersInRoom(pDnode);
  ShowObjsInRoom(pDnode);
  ShowMobsInRoom(pDnode);
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
  RoomFile.close();
}

/***********************************************************
* Move followers                                           *
************************************************************/

void Room::MoveFollowers(Dnode *pDnode, string ExitToRoomId)
{ // Recursive
  Dnode *pDnodeGrpMem;
  int    i;

  for (i = 1; i < GRP_LIMIT; i++)
  {
    if (!pDnode->pPlayer->pPlayerFollowers[i])
    { // No followers or no more followers
     return;
    }
    pDnodeGrpMem = GetTargetDnode(ConvertCStringToString(pDnode->pPlayer->pPlayerFollowers[i]->Name));
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

void Room::MovePlayer(Dnode *pDnode, string ExitToRoomId)
{
  CString csExitToRoomId;
  string  TmpStr;
  string  MoveMsg;
  CString csMoveMsg;

  pDnodeSrc = pDnode;
  pDnodeTgt = pDnode;
  // Leaves message
  if (MudCmd != "flee")
  { // If player is not fleeing
    MoveMsg = pDnode->PlayerName + " leaves.";
    csMoveMsg = ConvertStringToCString(MoveMsg);
    SendToRoom(pDnode->pPlayer->RoomId, csMoveMsg);
  }
  // Switch rooms
  pDnode->pPlayer->RoomIdBeforeMove = pDnode->pPlayer->RoomId;
  pDnode->pPlayer->RoomId = csExitToRoomId;
  Osi("Rooms", ExitToRoomId);
  pDnode->pPlayer->Save();
  // Arrives message
  MoveMsg = pDnode->PlayerName + " arrives.";
  SendToRoom(pDnode->pPlayer->RoomId, csMoveMsg);
}

/***********************************************************
* Open Room file                                           *
************************************************************/

bool Room::OpenFile(Dnode *pDnode)
{
  string  RoomFileName;

  RoomFileName =  ROOMS_DIR;
  RoomFileName += pDnode->pPlayer->RoomId;
  RoomFileName += ".txt";
  RoomFile.open(RoomFileName);
  if(RoomFile.is_open())
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
  string Stuff;
  CString csStuff;

  // RoomDesc
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 9) != "RoomDesc:")
  {
    AfxMessageBox("Room::ShowRoomDesc - RoomDesc: not found", MB_ICONSTOP);
    _endthread();
  }
  // Room Description
  getline(RoomFile, Stuff);
  while (Stuff != "End of RoomDesc")
  {
    csStuff = ConvertStringToCString(Stuff);
    pDnode->PlayerOut += csStuff;
    pDnode->PlayerOut += "\r\n";
    getline(RoomFile, Stuff);
  }
}

/***********************************************************
* Show exit description to player                          *
************************************************************/

void Room::ShowRoomExitDesc()
{
  string  Stuff;
  CString csStuff;

  // ExitDesc
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 9) != "ExitDesc:")
  {
    AfxMessageBox("Room::ShowRoomExitDesc - ExitDesc: not found", MB_ICONSTOP);
    _endthread();
  }
  // Exit Description
  getline(RoomFile, Stuff);
  while (StrLeft(Stuff, 13) != "ExitToRoomId:")
  {
    csStuff = ConvertStringToCString(Stuff);
    pDnodeActor->PlayerOut += csStuff;
    pDnodeActor->PlayerOut += "\r\n";
    getline(RoomFile, Stuff);
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
  string  Stuff;
  CString csStuff;

  NoExits = true;
  pDnode->PlayerOut += "&C";
  pDnode->PlayerOut += "Exits:";
  while (Stuff != "End of Exits")
  {
    getline(RoomFile, Stuff);
    if (StrLeft(Stuff, 9) == "ExitName:")
    {
      NoExits = false;
      Stuff = StrGetWord(Stuff, 2);
      pDnode->PlayerOut += " ";
      csStuff = ConvertStringToCString(Stuff);
      pDnode->PlayerOut += csStuff;
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
  string  RoomId;
  CString csRoomId;
  string  RoomType;
  CString csRoomType;
  string  Stuff;
  CString csStuff;
  string  Terrain;
  CString csTerrain;
  string  RoomName;
  CString csRoomName;

  // RoomId
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 7) != "RoomId:")
  {
    AfxMessageBox("Room::ShowRoomName - RoomId: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomId = StrGetWord(Stuff, 2);
  csRoomId = ConvertStringToCString(RoomId);
  if (csRoomId != pDnode->pPlayer->RoomId)
  {
    AfxMessageBox("Room::ShowRoomName - RoomId mis-match", MB_ICONSTOP);
    _endthread();
  }
  // RoomType
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 9) != "RoomType:")
  {
    AfxMessageBox("Room::ShowRoomName - RoomType: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomType = StrGetWords(Stuff, 2);
  // Terrain
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 8) != "Terrain:")
  {
    AfxMessageBox("Room::ShowRoomName - Terrain: not found", MB_ICONSTOP);
    _endthread();
  }
  Terrain = StrGetWord(Stuff, 2);
  csTerrain = ConvertStringToCString(Terrain);
  // RoomName
  getline(RoomFile, Stuff);
  if (StrLeft(Stuff, 9) != "RoomName:")
  {
    AfxMessageBox("Room::ShowRoomName - RoomName: not found", MB_ICONSTOP);
    _endthread();
  }
  RoomName = StrGetWords(Stuff, 2);
  StrTrimLeft(RoomName);
  csRoomName = ConvertStringToCString(RoomName);
  // Build player output
  pDnode->PlayerOut += "\r\n";
  pDnode->PlayerOut += "&C";
  pDnode->PlayerOut += csRoomName;
  pDnode->PlayerOut += "&N";
  if (pDnode->pPlayer->RoomInfo)
  { // Show hidden room info
    pDnode->PlayerOut += "&M";
    pDnode->PlayerOut += " [";
    pDnode->PlayerOut += "&N";
    pDnode->PlayerOut += csRoomId;
    pDnode->PlayerOut += " ";
    pDnode->PlayerOut += csTerrain;
    pDnode->PlayerOut += " ";
    pDnode->PlayerOut += csRoomType;
    pDnode->PlayerOut += "&M";
    pDnode->PlayerOut += "]";
    pDnode->PlayerOut += "&N";
  }
  pDnode->PlayerOut += "\r\n";
}