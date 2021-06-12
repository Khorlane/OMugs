/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Room.cpp                                         *
* Usage:  Handles all room commands                        *
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
#include "color.h"
#include "Player.h"
#include "Room.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern CString        GetWord(CString CmdStr, int WordNbr);
extern CString        GetWords(CString CmdStr, int WordNbr);
extern Communication  *pComm;

/***********************************************************
* Log constructor                                          *
************************************************************/

Room::Room()
{
}

/***********************************************************
* Log destructor                                           *
************************************************************/

Room::~Room()
{
}

/***********************************************************
* Go command - move the player                             *
************************************************************/

void Room::Move(Dnode *pDnode1)
{
  while (Stuff.Left(13) != "ExitToRoomId:")
  {
    ReadLine();
  }
  Stuff.Delete(0,13);
  pDnode1->pPlayer->RoomId = Stuff;
}

/***********************************************************
* If valid room exit, then deal with it                    *
************************************************************/

bool Room::IsExit(Dnode *pDnode1, CString CmdStr, CString MudCmd)
{
  bool    Found;
  CString ExitLookup;
  CString TmpStr;

  Found = false;
  if (!OpenFile(pDnode1))
  { // If the file isn't there, then the Room does not exit, doh!
    AfxMessageBox("Fatal error: Room does not exist");
    exit (-1);
  }
  if (MudCmd == "gogo")
  { // gogo is just a way of knowing that this is an implicit go
    MudCmd = "go";
    CmdStr = MudCmd + " " + CmdStr; // Make the exit the 2nd word
  }
  ExitLookup = GetWord(CmdStr, 2);
  ExitLookup.MakeLower();
  ExitLookup = TranslateExit(ExitLookup);
  Stuff = "Not Done";
  while (Stuff != "End of Room")
  { // Loop until Room is found or end of file
    ReadLine();
    if (Stuff.Left(9) == "ExitName:")
    { // Ok, an Exit has been found
      TmpStr = Stuff.Right(Stuff.GetLength() - 9);
      TmpStr.MakeLower();
      TmpStr = TranslateExit(TmpStr);
      if (TmpStr == ExitLookup)
      { // THE Exit has been found
        Found = true;
        Stuff = "End of Room";
      }
    }
  }
  if (Found)
  {
    if (MudCmd == "go")
    {
      Move(pDnode1);
      CloseFile();
      ShowRoom(pDnode1);
    }
    else
    {
      if (MudCmd == "look")
      {
        ShowExitDesc(pDnode1);
        CloseFile();
      }
    }
    return true; // so command processor will exit properly
  }
  else
  {
    CloseFile(); 
    return false; // so command processor will keep trying
  }
}

/***********************************************************
* Open Room file                                           *
************************************************************/

bool Room::OpenFile(Dnode *pDnode1)
{
  CString     FileName;
  int         Success;

  FileName =  ".\\Library\\Rooms\\";
  FileName += pDnode1->pPlayer->RoomId;
  FileName += ".txt";
  Success = RoomFile.Open(FileName,
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
* Close Room file                                          *
************************************************************/

void Room::CloseFile()
{
  RoomFile.Close();
}

/***********************************************************
* Read a line from Room file                               *
************************************************************/

void Room::ReadLine()
{
  RoomFile.ReadString(Stuff);
  Stuff.TrimLeft();
  Stuff.TrimLeft();
}

/***********************************************************
* Show the room to the player                              *
************************************************************/

void Room::ShowRoom(Dnode *pDnode1)
{
  if (!OpenFile(pDnode1))
  { // If the file isn't there, then the Room does not exit, doh!
    AfxMessageBox("Fatal error: Room does not exist");
    exit (-1);
  }
  ShowName(pDnode1);
  ShowDesc(pDnode1);
  ShowExits(pDnode1);
  CloseFile();
  pDnode1->PlayerOut += "\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/***********************************************************
* Show the room name to the player                         *
************************************************************/

void Room::ShowName(Dnode *pDnode1)
{
  // RoomId
  ReadLine();
  if (Stuff.Left(7) != "RoomId:")
  {
    AfxMessageBox("Fatal Error: RoomId: not found");
    exit (-1);
  }
  Stuff.Delete(0,7);
  if (Stuff != pDnode1->pPlayer->RoomId)
  {
    AfxMessageBox("Fatal Error: RoomId mis-match");
    exit (-1);
  }
  // RoomName
  ReadLine();
  if (Stuff.Left(9) != "RoomName:")
  {
    AfxMessageBox("Fatal Error: RoomName: not found");
    exit (-1);
  }
  Stuff.Delete(0,9);
  pDnode1->PlayerOut += "&C";
  pDnode1->PlayerOut += Stuff;
  pDnode1->PlayerOut += "\r\n";
  pDnode1->PlayerOut += "&N";
}

/***********************************************************
* Show the room description to the player                  *
************************************************************/

void Room::ShowDesc(Dnode *pDnode1)
{
  // RoomDesc
  ReadLine();
  if (Stuff.Left(9) != "RoomDesc:")
  {
    AfxMessageBox("Fatal Error: RoomDesc: not found");
    exit (-1);
  }
  // Room Description
  ReadLine();
  while (Stuff.Left(9) != "RoomType:")
  {
    pDnode1->PlayerOut += Stuff;
    pDnode1->PlayerOut += "\r\n";
    ReadLine();
  }
}

/***********************************************************
* Show the room exits to the player                        *
************************************************************/

void Room::ShowExits(Dnode *pDnode1)
{
  pDnode1->PlayerOut += "&C";
  pDnode1->PlayerOut += "Exits:";
  while (Stuff != "End of Room")
  {
    ReadLine();
    if (Stuff.Left(9) == "ExitName:")
    {
      Stuff.Delete(0,9);
      pDnode1->PlayerOut += " ";
      pDnode1->PlayerOut += Stuff;
    }
  }
  pDnode1->PlayerOut += "&N";
}

/***********************************************************
* Show exit description to player                          *
************************************************************/

void Room::ShowExitDesc(Dnode *pDnode1)
{
  // ExitDesc
  ReadLine();
  if (Stuff.Left(9) != "ExitDesc:")
  {
    AfxMessageBox("Fatal Error: ExitDesc: not found");
    exit (-1);
  }
  // Exit Description
  ReadLine();
  while (Stuff.Left(13) != "ExitToRoomId:")
  {
    pDnode1->PlayerOut += Stuff;
    pDnode1->PlayerOut += "\r\n";
    ReadLine();
  }
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}

/***********************************************************
* Translate exit name to from known abbreviations to words *
************************************************************/

CString Room::TranslateExit(CString ExitName)
{
  if (ExitName == "north")
  {
    return "n";
  }
  if (ExitName == "south")
  {
    return "s";
  }
  if (ExitName == "east")
  {
    return "e";
  }
  if (ExitName == "west")
  {
    return "w";
  }
  if (ExitName == "northeast")
  {
    return "ne";
  }
  if (ExitName == "southeast")
  {
    return "se";
  }
  if (ExitName == "northwest")
  {
    return "nw";
  }
  if (ExitName == "southwest")
  {
    return "sw";
  }
  return ExitName;
}