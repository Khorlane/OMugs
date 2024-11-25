/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Mobile.cpp                                       *
* Usage:  Manages mobiles                                  *
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
#include "Mobile.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* Mobile constructor                                       *
************************************************************/

Mobile::Mobile(string MobileId)
{
  OpenFile(ConvertStringToCString(MobileId));
  ParseStuff();
  CloseFile();
  Hurt = false;
  MobNbr = "";
}

/***********************************************************
* Mobile destructor                                        *
************************************************************/

Mobile::~Mobile()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Add a mobile to a room                                   *
************************************************************/

void Mobile::AddMobToRoom(string RoomId, string MobileId)
{
  int        BytesInFile;
  bool       NewRoomMobFile;
  int        MobCount;
  bool       MobileIdAdded;
  string     MobileIdCheck;
  ifstream   RoomMobFile;
  string     RoomMobFileName;
  ofstream   RoomMobTmpFile;
  string     RoomMobTmpFileName;
  string     Stuff;
  string     TmpStr;

  UpdateMobInWorld(ConvertStringToCString(MobileId), "add");
  MobileId = StrMakeLower(MobileId);
  // Open RoomMob file
  RoomMobFileName =  ROOM_MOB_DIR;
  RoomMobFileName += RoomId;
  RoomMobFileName += ".txt";
  NewRoomMobFile = false;
  RoomMobFile.open(RoomMobFileName);
  if(!RoomMobFile.is_open())
  {
    NewRoomMobFile = true;
  }
  // Open temp RoomMob file
  RoomMobTmpFileName =  ROOM_MOB_DIR;
  RoomMobTmpFileName += RoomId;
  if(RoomId == "")
  {
    AfxMessageBox("RoomId is blank 1", MB_ICONSTOP);
    _endthread();
  }
  RoomMobTmpFileName += ".tmp.txt";
  RoomMobTmpFile.open(RoomMobTmpFileName);
  if(!RoomMobTmpFile.is_open())
  {
    AfxMessageBox("Mobile::AddMobToRoom - Open RoomMob temp file failed", MB_ICONSTOP);
    _endthread();
  }
  if (NewRoomMobFile)
  { // New room mobile file, write the mobile and return
    TmpStr  = "1 ";
    TmpStr += MobileId;
    TmpStr += "\n";
    RoomMobTmpFile << TmpStr << endl;
    RoomMobTmpFile.close();
    Rename(RoomMobTmpFileName, RoomMobFileName);
    return;
  }
  // Write temp RoomMob file
  MobileIdAdded = false;
  getline(RoomMobFile, Stuff);
  while (Stuff != "")
  {
    if (MobileIdAdded)
    { // New mobile has been written, just write the rest of the mobiles
      Stuff += "\n";
      RoomMobTmpFile <<  Stuff << endl;
      getline(RoomMobFile, Stuff);
      continue;
    }
    MobileIdCheck = StrGetWord(Stuff, 2);
    if (MobileId < MobileIdCheck)
    { // Add new mobile in alphabetical order
      TmpStr  = "1 ";
      TmpStr += MobileId;
      TmpStr += "\n";
      RoomMobTmpFile << TmpStr << endl;
      MobileIdAdded = true;
      Stuff += "\n";
      RoomMobTmpFile << Stuff << endl;
      getline(RoomMobFile, Stuff);
      continue;
    }
    if (MobileId == MobileIdCheck)
    { // Existing mobile same as new mobile, add 1 to count
      MobCount = stoi(StrGetWord(Stuff, 1));
      MobCount++;
      sprintf(Buf, "%d", MobCount);
      TmpStr = Buf;
      TmpStr += " ";
      TmpStr += MobileId;
      TmpStr += "\n";
      RoomMobTmpFile << TmpStr << endl;
      MobileIdAdded = true;
      getline(RoomMobFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    RoomMobTmpFile << Stuff << endl;
    getline(RoomMobFile, Stuff);
  }
  if (!MobileIdAdded)
  { // New mobile goes at the end
    TmpStr  = "1 ";
    TmpStr += MobileId;
    TmpStr += "\n";
    RoomMobTmpFile << TmpStr << endl;
    MobileIdAdded = true;
  }
  BytesInFile = StrGetLength(RoomMobTmpFileName); // TODO - steve - What is this doing?
  RoomMobFile.close();
  RoomMobTmpFile.close();
  Remove(RoomMobFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    Rename(RoomMobTmpFileName, RoomMobFileName);
  }
  else
  { // If the file is empty, delete it for and abort ... it should never be empty
    Remove(RoomMobTmpFileName);
    AfxMessageBox("Mobile::AddMobToRoom RoomMob file size is not > 0!!", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Look mobiles InWorld count                               *
************************************************************/

int Mobile::CountMob(string MobileId)
{
  int        MobInWorldCount;
  ifstream   MobInWorldFile;
  string     MobInWorldFileName;
  string     Stuff;
  string     TmpStr;
  
  // Open Mobile InWorld file
  MobInWorldFileName =  CONTROL_MOB_INWORLD_DIR;
  MobInWorldFileName += MobileId;
  MobInWorldFileName += ".txt";
  MobInWorldFile.open(MobInWorldFileName);
  if(MobInWorldFile.is_open())
  { // Get current count
    getline(MobInWorldFile,Stuff);
    MobInWorldCount = stoi(Stuff);
    MobInWorldFile.close();
  }
  else
  { // No file, so count is zero
    MobInWorldCount = 0;
  }
  return MobInWorldCount;
}

/***********************************************************
* Create mobile player file                                *
************************************************************/

void Mobile::CreateMobPlayer(string PlayerName, string MobileId)
{
  bool       NewFile;
  fstream    MobPlayerFile;
  string     MobPlayerFileName;

  NewFile              = true;
  MobPlayerFileName    = MOB_PLAYER_DIR;
  MobPlayerFileName   += PlayerName;
  MobPlayerFileName   += ".txt";
  MobPlayerFile.open(MobPlayerFileName);
  if (MobPlayerFile.is_open())
  {
    NewFile = false;
    MobPlayerFile.close();
  }
  if (NewFile)
  { // Create new file
    MobPlayerFile.open(MobPlayerFileName);
    if (!MobPlayerFile.is_open())
    {
      AfxMessageBox("Mobile::CreateMobPlayer - Open MobPlayerile file failed 1", MB_ICONSTOP);
      _endthread();
    }
  }
  else
  { // Use existing file
    MobPlayerFile.open(MobPlayerFileName);
    if(!MobPlayerFile.is_open())
    {
      AfxMessageBox("Mobile::CreateMobPlayer - Open MobPlayerile file failed 2", MB_ICONSTOP);
      _endthread();
    }
  }
  if (!NewFile)
  {
    MobPlayerFile.seekg(0, std::ios::end);
    MobPlayerFile << "\r\n";
  }
  MobPlayerFile << MobileId << endl;
  MobPlayerFile.close();
}

/***********************************************************
* Create mobile statistics file - Write                    *
************************************************************/

void Mobile::CreateMobStatsFileWrite(string Directory, string MobileIdForFight, string Stuff)
{
  string     AfxMessage;
  ofstream   MobStatsFile;
  string     MobStatsFileName;

  MobStatsFileName =  Directory;
  MobStatsFileName += MobileIdForFight;
  MobStatsFileName += ".txt";
  MobStatsFile.open(MobStatsFileName);
  if (!MobStatsFile.is_open())
  { // Open file failed
    AfxMessage = "Mobile::CreateMobStatsFileWrite - Open for ";
    AfxMessage += Directory;
    AfxMessage += " ";
    AfxMessage += MobileIdForFight;
    AfxMessage += " failed.";
    AfxMessageBox(ConvertStringToCString(AfxMessage), MB_ICONSTOP);
    _endthread();
  }
  MobStatsFile << Stuff << endl;
  MobStatsFile.close();
}

/***********************************************************
* Create player mobile file                                *
************************************************************/

void Mobile::CreatePlayerMob(string PlayerName, string MobileId)
{
  ofstream   PlayerMobFile;
  string     PlayerMobFileName;

  PlayerMobFileName =  PLAYER_MOB_DIR;
  PlayerMobFileName += PlayerName;
  PlayerMobFileName += ".txt";
  PlayerMobFile.open(PlayerMobFileName);
  if (!PlayerMobFile.is_open())
  {
    AfxMessageBox("Mobile::CreatePlayerMob - Open PlayerMob file failed", MB_ICONSTOP);
    _endthread();
  }
  PlayerMobFile << MobileId << endl;
  PlayerMobFile.close();
}

/***********************************************************
* Delete mobile->player fight relationship                 *
************************************************************/

void Mobile::DeleteMobPlayer(string PlayerName, string MobileId)
{
  streamoff  BytesInFile;
  bool       MobileIdDeleted;
  string     MobileIdCheck;
  string     MobPlayerFileName;
  string     MobPlayerFileNameTmp;
  ifstream   MobPlayerFile;
  ofstream   MobPlayerFileTmp;
  string     Stuff;
  string     TmpStr;

  MobileId = StrMakeLower(MobileId);
  // Open MobPlayer file
  MobPlayerFileName =  MOB_PLAYER_DIR;
  MobPlayerFileName += PlayerName;
  MobPlayerFileName += ".txt";
  MobPlayerFile.open(MobPlayerFileName);
  if(!MobPlayerFile.is_open())
  { // MobPlayer player file does not exist
    return;
  }
  if (MobileId == "file")
  { // Delete the file
    MobPlayerFile.close();
    Remove(MobPlayerFileName);
    return;
  }
  // Open temp MobPlayer file
  MobPlayerFileNameTmp =  MOB_PLAYER_DIR;
  MobPlayerFileNameTmp += PlayerName;
  MobPlayerFileNameTmp += ".tmp.txt";
  MobPlayerFileTmp.open(MobPlayerFileNameTmp);
  if(!MobPlayerFileTmp.is_open())
  {
    AfxMessageBox("Mobile::DeleteMobPlayer - Open MobPlayer temp file failed", MB_ICONSTOP);
    _endthread();
  }
  // Write temp MobPlayer file
  MobileIdDeleted = false;
  getline(MobPlayerFile, Stuff);
  while (Stuff != "")
  {
    if (MobileIdDeleted)
    { // Mobile has been deleted, just write the rest of the mobiles
      Stuff += "\n";
      MobPlayerFileTmp << Stuff << endl;
      getline(MobPlayerFile, Stuff);
      continue;
    }
    MobileIdCheck = StrGetWord(Stuff, 1);
    MobileIdCheck = StrMakeLower(MobileIdCheck);
    if (MobileId == MobileIdCheck)
    { // Found it, delete it
      MobileIdDeleted = true;
      getline(MobPlayerFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    MobPlayerFileTmp << Stuff << endl;
    getline(MobPlayerFile, Stuff);
  }
  BytesInFile = MobPlayerFileTmp.tellp();
  MobPlayerFile.close();
  MobPlayerFileTmp.close();
  Remove(MobPlayerFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    Rename(MobPlayerFileNameTmp, MobPlayerFileName);
  }
  else
  { // If the file is empty, delete it
    Remove(MobPlayerFileNameTmp);
  }
}

/***********************************************************
* Delete mobile->statistics fight relationships            *
************************************************************/

void Mobile::DeleteMobStats(string MobileId)
{
  string MobStatsFileName;
  string PlayerMobFileName;

  // Delete 'MobStats' Armor file
  MobStatsFileName = MOB_STATS_ARM_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
  // Delete 'MobStats' Attack file
  MobStatsFileName = MOB_STATS_ATK_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
  // Delete 'MobStats' Damage file
  MobStatsFileName = MOB_STATS_DMG_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
  // Delete 'MobStats' Desc1 file
  MobStatsFileName = MOB_STATS_DSC_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
  // Delete 'MobStats' ExpPoints file
  MobStatsFileName = MOB_STATS_EXP_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
  // Delete 'MobStats' HitPoints file
  MobStatsFileName = MOB_STATS_HPT_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
  // Delete 'MobStats' Loot file
  MobStatsFileName = MOB_STATS_LOOT_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
  // Delete 'MobStats' Room file
  MobStatsFileName = MOB_STATS_ROOM_DIR;
  MobStatsFileName += MobileId;
  MobStatsFileName += ".txt";
  Remove(MobStatsFileName);
}

/***********************************************************
* Delete player->mobile fight relationship                 *
************************************************************/

void Mobile::DeletePlayerMob(CString PlayerName)
{
  CString MobStatsFileName;
  CString PlayerMobFileName;

  // Delete 'PlayerMob' file
  PlayerMobFileName =  PLAYER_MOB_DIR;
  PlayerMobFileName += PlayerName;
  PlayerMobFileName += ".txt";
  CFile::Remove(PlayerMobFileName);
}

/***********************************************************
* Is mobile in room                                        *
************************************************************/

Mobile *Mobile::IsMobInRoom(CString MobileName)
{
  Mobile     *pMobile;
  CString     NamesCheck;
  bool        MobileHurt;
  CString     MobileId;
  CString     MobileIdCheck;
  CString     MobileIdHurt;
  CString     MobileNameCheck;
  CString     MobNbr;
  int         PositionOfDot;
  CStdioFile  RoomMobFile;
  CString     RoomMobFileName;
  CString     Stuff;
  int         Success;

  // Open RoomMob file
  RoomMobFileName =  ROOM_MOB_DIR;
  RoomMobFileName += pDnodeActor->pPlayer->RoomId;
  RoomMobFileName += ".txt";
  //*******************************
  //* Try matching using MobileId *
  //*******************************
  Success = RoomMobFile.Open(RoomMobFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // Room has no mobiles
    return NULL;
  }
  RoomMobFile.ReadString(Stuff);
  while (Stuff != "")
  { // Process each mobile in the room
    MobileId = StrGetWord(Stuff, 2);
    if (MobileId == MobileName)
    { // This mobile is a match
      RoomMobFile.Close();
      PositionOfDot = StrFindFirstChar(MobileId, '.');
      MobileHurt    = false;
      if (PositionOfDot > 1)
      { // Mobile is hurt but not fighting
        MobileHurt   = true;
        MobileIdHurt = MobileId;
        MobNbr       = StrRight(MobileId, StrGetLength(MobileId) - PositionOfDot - 1);
        MobileId     = StrLeft(MobileId, PositionOfDot);
      }
      pMobile = new Mobile(ConvertCStringToString(MobileId));
      pMobile->Hurt   = MobileHurt;
      pMobile->MobNbr = MobNbr;
      return pMobile;
    }
    RoomMobFile.ReadString(Stuff);
  }
  RoomMobFile.Close();
  //***************************************************
  //* No match found, try getting match using 'names' *
  //***************************************************
  Success = RoomMobFile.Open(RoomMobFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // Room has no mobiles
    return NULL;
  }
  RoomMobFile.ReadString(Stuff);
  while (Stuff != "")
  { // Process each mobile in the room
    MobileId      = StrGetWord(Stuff, 2);
    PositionOfDot = StrFindFirstChar(MobileId, '.');
    MobileHurt    = false;
    if (PositionOfDot > 1)
    { // Mobile is hurt but not fighting
      MobileHurt   = true;
      MobileIdHurt = MobileId;
      MobNbr       = StrRight(MobileId, StrGetLength(MobileId) - PositionOfDot - 1);
      MobileId     = StrLeft(MobileId, PositionOfDot);
    }
    pMobile = new Mobile(ConvertCStringToString(MobileId));
    pMobile->Hurt    = MobileHurt;
    pMobile->MobNbr  = MobNbr;
    if (pMobile->Hurt)
    { // Mobile is hurt
      if (MobNbr == MobileName)
      { // Kill nnn was entered, where nnn is the MobNbr
        RoomMobFile.Close();
        return pMobile;
      }
    }
    NamesCheck = pMobile->Names;
    NamesCheck = StrMakeLower(NamesCheck);
    if (IsWord(MobileName, NamesCheck))
    { // This mobile is a match
      RoomMobFile.Close();
      return pMobile;
    }
    else
    { // This mobile doesn't match
      delete pMobile;
    }
    RoomMobFile.ReadString(Stuff);
  }
  RoomMobFile.Close();
  return NULL;
}

/***********************************************************
* Get mobile get Desc1                                     *
************************************************************/

CString Mobile::GetMobDesc1(CString MobileId)
{
  CString    Desc1;
  CStdioFile MobileFile;
  CString    MobileFileName;
  int        PositionOfDot;
  CString    Stuff;
  int        Success;

  PositionOfDot = StrFindFirstChar(MobileId, '.');
  if (PositionOfDot > 1)
  { // Mobile is hurt but not fighting
    MobileId = StrLeft(MobileId, PositionOfDot);
  }
  MobileFileName =  MOBILES_DIR;
  MobileFileName += MobileId;
  MobileFileName += ".txt";
  Success = MobileFile.Open(MobileFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Mobile::GetMobDesc1 - Mobile does not exist!", MB_ICONSTOP);
    _endthread();
  }
  Stuff = "";
  while (StrLeft(Stuff, 6) != "Desc1:")
  {
    MobileFile.ReadString(Stuff);
  }
  Desc1 = StrRight(Stuff, StrGetLength(Stuff)-6);
  Desc1 = StrTrimLeft(Desc1);
  MobileFile.Close();
  return Desc1;
}

/***********************************************************
* Is mobile in the room?                                   *
************************************************************/

bool Mobile::IsMobileIdInRoom(CString RoomId, CString MobileId)
{
  CString    MobileIdCheck;
  CStdioFile RoomMobFile;
  CString    RoomMobFileName;
  CString    Stuff;
  int        Success;

  // Open RoomMob file
  RoomMobFileName =  ROOM_MOB_DIR;
  RoomMobFileName += RoomId;
  RoomMobFileName += ".txt";
  Success = RoomMobFile.Open(RoomMobFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // Room has no mobiles
    return false;
  }
  RoomMobFile.ReadString(Stuff);
  while (Stuff != "")
  { // Process each mobile in the room
    MobileIdCheck = StrGetWord(Stuff, 2);
    if (MobileId == MobileIdCheck)
    { // Found matching mobile
      RoomMobFile.Close();
      return true;
    }
    RoomMobFile.ReadString(Stuff);
  }
  // No matching mobile found
  RoomMobFile.Close();
  return false;
}

/***********************************************************
* Is this a valid mobile?                                  *
************************************************************/

Mobile *Mobile::IsMobValid(string MobileId)
{
  Mobile     *pMobile;
  CString     MobileFileName;
  CStdioFile  MobileFile;
  int         Success;

  MobileFileName =  MOBILES_DIR;
  MobileFileName += ConvertStringToCString(MobileId);
  MobileFileName += ".txt";
  Success = MobileFile.Open(MobileFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(Success)
  {
    MobileFile.Close();
    pMobile = new Mobile(MobileId);
    return pMobile;
  }
  else
  {
    return NULL;
  }
}

/***********************************************************
* Put non-fighting mobiles back in room                    *
************************************************************/

void Mobile::PutMobBackInRoom(CString PlayerName, CString RoomIdBeforeFleeing)
{
  CString    MobHitPointsLeft;
  CString    MobHitPointsTotal;
  CString    MobileId;
  CString    MobPlayerFileName;
  CStdioFile MobPlayerFile;
  CStdioFile MobStatsHitPointsFile;
  CString    MobStatsHitPointsFileName;
  int        PositionOfDot;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  // Open MobPlayer file
  MobPlayerFileName =  MOB_PLAYER_DIR;
  MobPlayerFileName += PlayerName;
  MobPlayerFileName += ".txt";
  Success = MobPlayerFile.Open(MobPlayerFileName,
                    CFile::modeRead |
                    CFile::typeText);
  if(!Success)
  { // No mobiles to put back, someone else may be fighting the mob
    return;
  }
  // For each mobile still in MobPlayer file(non-fighting mobiles), put it back in room
  MobPlayerFile.ReadString(Stuff);
  while (Stuff != "")
  {
    MobileId = StrGetWord(Stuff, 1);
    MobileId = StrMakeLower(MobileId);
    // Read mobile stats hit points file
    MobStatsHitPointsFileName = MOB_STATS_HPT_DIR;
    MobStatsHitPointsFileName += MobileId;
    MobStatsHitPointsFileName += ".txt";
    Success = MobStatsHitPointsFile.Open(MobStatsHitPointsFileName,
                              CFile::modeRead |
                              CFile::typeText);
    if(!Success)
    {
      AfxMessageBox("Mobile::PutMobBackInRoom - Open MobStatsHitPointsFile file failed (read)", MB_ICONSTOP);
      _endthread();
    }
    MobStatsHitPointsFile.ReadString(Stuff);
    MobStatsHitPointsFile.Close();
    MobHitPointsTotal = StrGetWord(Stuff, 1);
    MobHitPointsLeft  = StrGetWord(Stuff, 2);
    if (MobHitPointsTotal == MobHitPointsLeft)
    { // Mobile is not hurt
      DeleteMobStats(ConvertCStringToString(MobileId));
      PositionOfDot = StrFindFirstChar(MobileId, '.');
      if (PositionOfDot > 1)
      { // Get MobileId
        MobileId = StrLeft(MobileId, PositionOfDot);
      }
    }
    AddMobToRoom(ConvertCStringToString(RoomIdBeforeFleeing), ConvertCStringToString(MobileId));
    UpdateMobInWorld(MobileId, "remove");
    MobPlayerFile.ReadString(Stuff);
  }
  MobPlayerFile.Close();
  CFile::Remove(MobPlayerFileName);
}

/***********************************************************
* Remove a mobile from room                                *
************************************************************/

void Mobile::RemoveMobFromRoom(string RoomId, string MobileId)
{
  streamoff  BytesInFile;
  bool       MobileIdRemoved;
  string     MobileIdCheck;
  int        MobCount;
  string     RoomMobFileName;
  string     RoomMobTmpFileName;
  ifstream   RoomMobFile;
  ofstream   RoomMobTmpFile;
  string     Stuff;
  string     TmpStr;

  UpdateMobInWorld(ConvertStringToCString(MobileId), "remove");
  MobileId = StrMakeLower(MobileId);
  // Open RoomMob file
  RoomMobFileName =  ROOM_MOB_DIR;
  RoomMobFileName += RoomId;
  RoomMobFileName += ".txt";
  RoomMobFile.open(RoomMobFileName);
  if(!RoomMobFile.is_open())
  {
    AfxMessageBox("Mobile::RemoveMobFromRoom - Open RoomMob file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open temp RoomMob file
  RoomMobTmpFileName =  ROOM_MOB_DIR;
  RoomMobTmpFileName += RoomId;
  if(RoomId == "")
  {
    AfxMessageBox("RoomId is blank 2", MB_ICONSTOP);
    _endthread();
  }
  RoomMobTmpFileName += ".tmp.txt";
  RoomMobTmpFile.open(RoomMobTmpFileName);
  if(!RoomMobTmpFile.is_open())
  {
    AfxMessageBox("Mobile::RemoveMobFromRoom - Open RoomMob temp file failed", MB_ICONSTOP);
    _endthread();
  }
  // Write temp RoomMob file
  MobileIdRemoved = false;
  getline(RoomMobFile, Stuff);
  while (Stuff != "")
  {
    if (MobileIdRemoved)
    { // Mobile has been removed, just write the rest of the mobiles
      Stuff += "\n";
      RoomMobTmpFile << Stuff << endl;
      getline(RoomMobFile, Stuff);
      continue;
    }
    MobileIdCheck = StrGetWord(Stuff, 2);
    if (MobileId == MobileIdCheck)
    { // Found it, subtract 1 from count
      MobCount = stoi(StrGetWord(Stuff, 1));
      MobCount--;
      MobileIdRemoved = true;
      if (MobCount > 0)
      {
        sprintf(Buf, "%d", MobCount);
        TmpStr = ConvertStringToCString(Buf);
        MobileId = TmpStr + " " + MobileId;
        MobileId += "\n";
        RoomMobTmpFile << Stuff << endl;
      }
      getline(RoomMobFile, Stuff);
      continue;
    }
    // None of the above conditions satisfied, just write it
    Stuff += "\n";
    RoomMobTmpFile << Stuff << endl;
    getline(RoomMobFile, Stuff);
  }
  if (!MobileIdRemoved)
  { // Mobile not removed, this is definitely BAD!
    AfxMessageBox("Mobile::RemoveMobFromRoom - Mobile not removed", MB_ICONSTOP);
    _endthread();
  }
  BytesInFile = RoomMobTmpFile.tellp();
  RoomMobFile.close();
  RoomMobTmpFile.close();
  Remove(RoomMobFileName);
  if (BytesInFile > 0)
  { // If the file is not empty, rename it
    Rename(RoomMobTmpFileName, RoomMobFileName);
  }
  else
  { // If the file is empty, delete it
    Remove(RoomMobTmpFileName);
  }
}

/***********************************************************
* Show room mobiles                                        *
************************************************************/

void Mobile::ShowMobsInRoom(Dnode *pDnode)
{
  Mobile     *pMobile;
  int         i;
  int         j;
  CString     MobileCount;
  bool        MobileHurt;
  CString     MobileId;
  CString     MobileIdsToBeRemoved;
  CString     MobileIdHurt;
  CString     MobNbr;
  int         PositionOfDot;
  int         RemoveMobCount;
  CStdioFile  RoomMobFile;
  CString     RoomMobFileName;
  CString     Stuff;
  int         Success;

  // Open RoomMob file
  RoomMobFileName =  ROOM_MOB_DIR;
  RoomMobFileName += pDnode->pPlayer->RoomId;
  RoomMobFileName += ".txt";
  Success = RoomMobFile.Open(RoomMobFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // No mobiles in room to display
    return;
  }
  RoomMobFile.ReadString(Stuff);
  while (Stuff != "")
  {
    MobileCount = StrGetWord(Stuff, 1);
    MobileId    = StrGetWord(Stuff, 2);
    PositionOfDot = StrFindFirstChar(MobileId, '.');
    MobileHurt = false;
    if (PositionOfDot > 1)
    { // Mobile is hurt but not fighting
      MobileHurt = true;
      MobileIdHurt = MobileId;
      MobNbr = StrRight(MobileId, StrGetLength(MobileId) - PositionOfDot - 1);
      MobileId = StrLeft(MobileId, PositionOfDot);
    }
    pMobile = new Mobile(ConvertCStringToString(MobileId));
    pMobile->Hurt = MobileHurt;
    pMobile->MobNbr = MobNbr;
    if (MobileHurt)
    { // Mobile is hurt
      pDnode->PlayerOut += "\r\n";
      pDnode->PlayerOut += "&W";
      pDnode->PlayerOut += "You see ";
      pDnode->PlayerOut += pMobile->Desc1;
      pDnode->PlayerOut += ", ";
      pDnode->PlayerOut += "&M";
      pDnode->PlayerOut += "wounded";
      pDnode->PlayerOut += "&W";
      pDnode->PlayerOut +=  ", trying to hide.";
      pDnode->PlayerOut += "&B";
      pDnode->PlayerOut += " (";
      pDnode->PlayerOut += MobileIdHurt;
      pDnode->PlayerOut += ")";
      pDnode->PlayerOut += "&N";
    }
    else
    { // Mobile is not hurt
      pDnode->PlayerOut += "\r\n";
      pDnode->PlayerOut += "&W";
      pDnode->PlayerOut += "(" + MobileCount + ") ";
      pDnode->PlayerOut += pMobile->Desc2;
      pDnode->PlayerOut += "&N";
    }
    // Check for AGGRO mobs
    if (IsWord("Aggro", pMobile->Action))
    { // Attack player
      j = atoi(MobileCount);
      for (i = 1; i <= j; i++)
      {
        MobileIdsToBeRemoved += MobAttacks(pMobile);
        MobileIdsToBeRemoved += " ";
      }
    }
    delete pMobile;
    RoomMobFile.ReadString(Stuff);
  }
  RoomMobFile.Close();
  // Remove mobs, that attacked a player, from room
  RemoveMobCount = StrCountWords(MobileIdsToBeRemoved);
  for (i = 1; i <= RemoveMobCount; i++)
  {
    MobileId = StrGetWord(MobileIdsToBeRemoved, i);
    RemoveMobFromRoom(ConvertCStringToString(pDnode->pPlayer->RoomId), ConvertCStringToString(MobileId));
  }
}

/***********************************************************
* Mobile attacks!!                                         *
************************************************************/

CString Mobile::MobAttacks(Mobile *pMobile)
{
  CString KillMsg;
  CString MobileId;
  CString MobileIdToBeRemoved;
  CString PhraseAll;
  CString PhrasePlayer;
  CString PlayerName;
  CString RoomId;

  PlayerName = pDnodeActor->PlayerName;
  RoomId     = pDnodeActor->pPlayer->RoomId;
  //*****************
  //* Send messages *
  //*****************
  // Determine appropriate phrase
  if (!pDnodeActor->PlayerStateFighting)
  { // Phrases for starting a fight
    PhrasePlayer = " starts a fight with you!";
    PhraseAll    = " starts a fight with ";
  }
  else
  { // Phrases for mob attacking a player already fighting]
    PhrasePlayer = " attacks you!";
    PhraseAll    = " attacks ";
  }
  // Send message to player
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "&R";
  pDnodeActor->PlayerOut += pMobile->Desc1;
  pDnodeActor->PlayerOut += PhrasePlayer;
  pDnodeActor->PlayerOut += "&N";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
  // Send message to room
  KillMsg = "&R";
  KillMsg += pMobile->Desc1;
  KillMsg += PhraseAll;
  KillMsg += PlayerName;
  KillMsg += "!";
  KillMsg += "&N";
  pDnodeSrc = pDnodeActor;
  pDnodeTgt = pDnodeActor;
  SendToRoom(RoomId, KillMsg);
  //*****************
  //* Start a fight *
  //*****************
  if (!pMobile->Hurt)
  { //  Mobile not hurt
    pMobile->GetNextMobNbr();
    pMobile->CreateMobStatsFile(RoomId);
    MobileId = pMobile->MobileId;
    MobileIdToBeRemoved = MobileId; // RemoveMobFromRoom(RoomId, MobileId);
    MobileId = pMobile->MobileId + "." + pMobile->MobNbr;
  }
  else
  { // Mobile is hurt
    MobileId = pMobile->MobileId + "." + pMobile->MobNbr;
    MobileIdToBeRemoved = MobileId;// RemoveMobFromRoom(RoomId, MobileId);
  }
  if (!pDnodeActor->PlayerStateFighting)
  { // Set player and mobile to fight
    CreatePlayerMob(ConvertCStringToString(PlayerName), ConvertCStringToString(MobileId));
    CreateMobPlayer(ConvertCStringToString(PlayerName), ConvertCStringToString(MobileId));
    pDnodeActor->PlayerStateFighting = true;
  }
  else
  { // Player is fighting, this mob is an 'add'
    CreateMobPlayer(ConvertCStringToString(PlayerName), ConvertCStringToString(MobileId));
  }
  return MobileIdToBeRemoved;
}

/***********************************************************
* Search all rooms for mobile                              *
************************************************************/

void Mobile::WhereMob(CString MobileIdSearch)
{
  CFileFind  FileList;
  CString    FileName;
  bool       MobileHurt;
  CString    MobileId;
  BOOL       MoreFiles;
  int        PositionOfDot;
  CString    RoomMobFileName;
  CStdioFile RoomMobFile;
  CString    RoomName;
  CString    Stuff;
  int        Success;

  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "Mobiles";
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->PlayerOut += "-------";
  pDnodeActor->PlayerOut += "\r\n";
  if (ChgDir(ROOM_MOB_DIR))
  { // Change directory failed
    AfxMessageBox("Mobile::WhereMob - Change directory to ROOM_MOB_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  MoreFiles = FileList.FindFile("*.*");
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    {
      continue;
    }
    FileName = FileList.GetFileName();
    // Open RoomMob file
    RoomMobFileName = FileName;
    Success = RoomMobFile.Open(RoomMobFileName,
                    CFile::modeRead |
                    CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("Mobile::WhereMob - Open RoomMob file failed", MB_ICONSTOP);
      _endthread();
    }
    RoomName = StrLeft(FileName, StrGetLength(FileName) - 4);
    RoomMobFile.ReadString(Stuff);
    while (Stuff != "")
    {
      MobileId      = StrGetWord(Stuff, 2);
      PositionOfDot = StrFindFirstChar(MobileId, '.');
      MobileHurt    = false;
      if (PositionOfDot > 1)
      { // Mobile is hurt but not fighting
        MobileHurt = true;
        MobileId   = StrLeft(MobileId, PositionOfDot);
      }
      if (MobileId == MobileIdSearch)
      {
        pDnodeActor->PlayerOut += RoomName;
        pDnodeActor->PlayerOut += " ";
        if (MobileHurt)
        {
          pDnodeActor->PlayerOut += "&R";
        }
        pDnodeActor->PlayerOut += Stuff;
        pDnodeActor->PlayerOut += "&N";
        pDnodeActor->PlayerOut += "\r\n";
      }
      RoomMobFile.ReadString(Stuff);
    }
    RoomMobFile.Close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("Mobile::WhereMob - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Update mobiles in the world count                        *
************************************************************/

void Mobile::UpdateMobInWorld(CString MobileId, CString AddRemove)
{
  int        MobInWorldCount;
  CStdioFile MobInWorldFile;
  CString    MobInWorldFileName;
  int        PositionOfDot;
  CString    Stuff;
  int        Success;
  CString    TmpStr;
  
  MobInWorldCount = 0;
  PositionOfDot = StrFindFirstChar(MobileId, '.');
  if (PositionOfDot > 1)
  { // Get MobileId
    MobileId = StrLeft(MobileId, PositionOfDot);
  }
  // Open Mobile InWorld file
  MobInWorldFileName =  CONTROL_MOB_INWORLD_DIR;
  MobInWorldFileName += MobileId;
  MobInWorldFileName += ".txt";
  Success = MobInWorldFile.Open(MobInWorldFileName,
                     CFile::modeRead |
                     CFile::typeText);
  if(Success)
  { // Get current count
    MobInWorldFile.ReadString(Stuff);
    MobInWorldCount = atoi(Stuff);
    MobInWorldFile.Close();
  }
  // Create Mobiles InWorld file, doesn't matter if it already exists
  Success = MobInWorldFile.Open(MobInWorldFileName,
                  CFile::modeCreate |
                  CFile::modeWrite  |
                  CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Mobile::UpdateMobInWorld - Open Mobiles InWorld file failed for: " + MobInWorldFileName, MB_ICONSTOP);
    _endthread();
  }
  if (AddRemove == "add")
  { // Mobile is being added to the world
    MobInWorldCount++;
  }
  else
  { // Mobile is being removed from the world
    MobInWorldCount--;
  }
  sprintf(Buf, "%d", MobInWorldCount);
  TmpStr = ConvertStringToCString(Buf);
  MobInWorldFile.WriteString(TmpStr);
  MobInWorldFile.Close();
}

////////////////////////////////////////////////////////////
// Public functions                                       //
////////////////////////////////////////////////////////////

/***********************************************************
* Create mobile statistics file                            *
************************************************************/

void Mobile::CreateMobStatsFile(CString RoomId)
{
  CString MobileIdForFight;
  CString Stuff;
  CString TmpStr;

  MobileIdForFight = MobileId + "." + MobNbr;
  // HitPoints
  sprintf(Buf, "%d", HitPoints);
  TmpStr = ConvertStringToCString(Buf);
  Stuff = TmpStr;
  Stuff += " ";
  Stuff += TmpStr;
  CreateMobStatsFileWrite(MOB_STATS_HPT_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
  // Armor
  sprintf(Buf, "%d", Armor);
  Stuff = ConvertStringToCString(Buf);
  CreateMobStatsFileWrite(MOB_STATS_ARM_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
  // Attack
  Stuff= Attack;
  CreateMobStatsFileWrite(MOB_STATS_ATK_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
  // Damage
  sprintf(Buf, "%d", Damage);
  Stuff = ConvertStringToCString(Buf);
  CreateMobStatsFileWrite(MOB_STATS_DMG_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
  // Desc1
  Stuff = Desc1;
  CreateMobStatsFileWrite(MOB_STATS_DSC_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
  // ExpPoints
  sprintf(Buf, "%d", ExpPoints);
  Stuff = ConvertStringToCString(Buf);
  sprintf(Buf, "%d", Level);
  TmpStr = ConvertStringToCString(Buf);
  Stuff += " ";
  Stuff += TmpStr;
  CreateMobStatsFileWrite(MOB_STATS_EXP_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
  // Loot
  Stuff = Loot;
  CreateMobStatsFileWrite(MOB_STATS_LOOT_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
  // Room
  Stuff = RoomId;
  CreateMobStatsFileWrite(MOB_STATS_ROOM_DIR, ConvertCStringToString(MobileIdForFight), ConvertCStringToString(Stuff));
}

/***********************************************************
* Examine a mobile, get Desc3                              *
************************************************************/

void Mobile::ExamineMob(CString MobileId)
{
  OpenFile(MobileId);
  while (Stuff != "Desc3:")
  {
    MobileFile.ReadString(Stuff); // Do not use ReadLine() here
  }
  // Mobile Description 3
  MobileFile.ReadString(Stuff); // Do not use ReadLine() here
  while (Stuff != "End Desc3")
  {
    pDnodeActor->PlayerOut += Stuff;
    pDnodeActor->PlayerOut += "\r\n";
    MobileFile.ReadString(Stuff); // Do not use ReadLine() here
  }
  pDnodeActor->PlayerOut += "&N";
  CloseFile();
}

/***********************************************************
* Get next mobile number                                   *
************************************************************/
void Mobile::GetNextMobNbr()
{
  CString    NextMobNbr;
  CStdioFile NextMobNbrFile;
  CString    NextMobNbrFileName;
  int        NextMobNbrInteger;
  CString    NextMobNbrString;
  CString    Stuff;
  int        Success;

  // Read next mobile number file
  NextMobNbrFileName = CONTROL_DIR;
  NextMobNbrFileName += "NextMobileNumber";
  NextMobNbrFileName += ".txt";
  Success = NextMobNbrFile.Open(NextMobNbrFileName,
                     CFile::modeRead |
                     CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Mobile::GetNextMobNbr - Open NextMobileNumber file failed (read)", MB_ICONSTOP);
    _endthread();
  }
  NextMobNbrFile.ReadString(Stuff);
  NextMobNbrFile.Close();
  // Increment next mobile number
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
  NextMobNbr        = Stuff;
  NextMobNbrInteger = atoi(Stuff);
  NextMobNbrInteger++;
  sprintf(Buf, "%d", NextMobNbrInteger);
  NextMobNbrString = ConvertStringToCString(Buf);
  // Write next mobile number file
  Success = NextMobNbrFile.Open(NextMobNbrFileName,
                     CFile::modeWrite |
                     CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Mobile::GetNextMobNbr - Open NextMobileNumber file failed (write)", MB_ICONSTOP);
    _endthread();
  }
  NextMobNbrFile.WriteString(NextMobNbrString);
  NextMobNbrFile.Close();
  // Set mobile number
  MobNbr = NextMobNbr;
}

/***********************************************************
* Make the mobiles talk                                    *
************************************************************/

CString Mobile::MobTalk()
{
  CString    LogBuf;
  CStdioFile MobTalkFile;
  CString    MobTalkFileName;
  CString    MobileMsg;
  int        MsgCount;
  int        RndMsgNbr;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  //******************************
  //* Open and read message file *
  //******************************
  MobTalkFileName =  TALK_DIR;
  MobTalkFileName += Talk;
  MobTalkFileName += ".txt";
  Success = MobTalkFile.Open(MobTalkFileName,
                  CFile::modeRead |
                  CFile::typeText);
  if(!Success)
  { // Open failed
    if (Talk != "None")
    { // Talk is not 'None', so file should exist
      LogBuf  = "Mobile::MobTalk - Failed to open ";
      LogBuf += MobTalkFileName;
      LogIt(LogBuf);
    }
    MobileMsg = "You are ignored.\r\n";
    return MobileMsg;
  }
  // Mobile is going to talk
  MobileMsg  = "&W";
  MobileMsg += StrMakeFirstUpper(Desc1);
  MobileMsg += " says:";
  MobileMsg += "&N";
  MobileMsg += "\r\n";
  // Select random message number
  MobTalkFile.ReadString(Stuff);
  MsgCount = atoi(StrGetWord(Stuff, 4));
  RndMsgNbr = GetRandomNumber(MsgCount);
  // Search for selected message number
  MobTalkFile.ReadString(Stuff);
  while (atoi(StrGetWord(Stuff, 2)) != RndMsgNbr)
  { // Find the selected message
    if (MobTalkFile.GetPosition() == MobTalkFile.GetLength())
    { // End of file and message was not found
      sprintf(Buf, "%d", RndMsgNbr);
      TmpStr = ConvertStringToCString(Buf);
      LogBuf  = "Mobile::MobTalk - Failed to find message ";
      LogBuf += TmpStr;
      LogBuf += " ";
      LogBuf += MobTalkFileName;
      LogIt(LogBuf);
      MobTalkFile.Close();
      MobileMsg = "You are ignored.\r\n";
      return MobileMsg;
    }
    MobTalkFile.ReadString(Stuff);
  }
  // Message found
  MobTalkFile.ReadString(Stuff);
  while (Stuff != "End of Message")
  { // Read the message
    if (MobTalkFile.GetPosition() == MobTalkFile.GetLength())
    { // End of file and normal end of message not found
      sprintf(Buf, "%d", RndMsgNbr);
      TmpStr = ConvertStringToCString(Buf);
      LogBuf  = "Mobile::MobTalk - Unexpect end of file reading message ";
      LogBuf += TmpStr;
      LogBuf += " ";
      LogBuf += MobTalkFileName;
      LogIt(LogBuf);
      MobTalkFile.Close();
      MobileMsg = "You are ignored.\r\n";
      return MobileMsg;
    }
    MobileMsg += Stuff;
    MobileMsg += "\r\n";
    MobTalkFile.ReadString(Stuff);
  }
  MobTalkFile.Close();
  return MobileMsg;
}

////////////////////////////////////////////////////////////
// Private functions                                      //
////////////////////////////////////////////////////////////

/***********************************************************
* Close Mobile file                                        *
************************************************************/

void Mobile::CloseFile()
{
  MobileFile.Close();
}

/***********************************************************
* Open Mobile file                                         *
************************************************************/

void Mobile::OpenFile(CString MobileId)
{
  CString MobileFileName;
  int     Success;

  MobileFileName =  MOBILES_DIR;
  MobileFileName += MobileId;
  MobileFileName += ".txt";
  Success = MobileFile.Open(MobileFileName,
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Mobile::OpenFile - Mobile does not exist!", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Parse mobile stuff                                       *
************************************************************/

void Mobile::ParseStuff()
{
  ReadLine();
  while (Stuff != "")
  {
    if (StrLeft(Stuff, 9) == "MobileId:")
    {
      MobileId = StrRight(Stuff, StrGetLength(Stuff) - 9);
      MobileId = StrTrimLeft(MobileId);
    }
    else
    if (StrLeft(Stuff, 6) == "Names:")
    {
      Names = StrRight(Stuff, StrGetLength(Stuff) - 6);
      Names = StrTrimLeft(Names);
    }
    else
    if (StrLeft(Stuff, 6) == "Desc1:")
    {
      Desc1 = StrRight(Stuff, StrGetLength(Stuff) - 6);
      Desc1 = StrTrimLeft(Desc1);
    }
    else
    if (StrLeft(Stuff, 6) == "Desc2:")
    {
      Desc2 = StrRight(Stuff, StrGetLength(Stuff) - 6);
      Desc2 = StrTrimLeft(Desc2);
    }
    else
    if (StrLeft(Stuff, 6) == "Desc3:")
    {
    // Desc3 can be multi-line and is dealt with in 'ExamineMob'
    }
    else
    if (StrLeft(Stuff, 7) == "Action:")
    {
      Action = StrRight(Stuff, StrGetLength(Stuff) - 7);
      Action = StrTrimLeft(Action);
    }
    else
    if (StrLeft(Stuff, 8) == "Faction:")
    {
      Faction = StrRight(Stuff, StrGetLength(Stuff) - 8);
      Faction = StrTrimLeft(Faction);
    }
    else
    if (StrLeft(Stuff, 6) == "Level:")
    {
      Level = atoi(StrRight(Stuff, StrGetLength(Stuff) - 6));
    }
    else
    if (StrLeft(Stuff, 10) == "HitPoints:")
    {
      HitPoints  = atoi(StrRight(Stuff, StrGetLength(Stuff) - 10));
      HitPoints += Level * MOB_HPT_PER_LEVEL;
    }
    else
    if (StrLeft(Stuff, 6) == "Armor:")
    {
      Armor  = atoi(StrRight(Stuff, StrGetLength(Stuff) - 6));
      Armor += Level * MOB_ARM_PER_LEVEL;
    }
    else
    if (StrLeft(Stuff, 7) == "Attack:")
    {
      Attack = StrRight(Stuff, StrGetLength(Stuff) - 7);
      Attack = StrTrimLeft(Attack);
      Attack = StrMakeLower(Attack);
    }
    else
    if (StrLeft(Stuff, 7) == "Damage:")
    {
      Damage  = atoi(StrRight(Stuff, StrGetLength(Stuff) - 7));
      Damage += Level * MOB_DMG_PER_LEVEL;
    }
    else
    if (StrLeft(Stuff, 10) == "ExpPoints:")
    {
      ExpPoints  = atoi(StrRight(Stuff, StrGetLength(Stuff) - 10));
      ExpPoints += Level * MOB_EXP_PER_LEVEL;
    }
    else
    if (StrLeft(Stuff, 5) == "Loot:")
    {
      Loot = StrRight(Stuff, StrGetLength(Stuff) - 5);
      Loot = StrTrimLeft(Loot);
    }
    else
    if (StrLeft(Stuff, 5) == "Talk:")
    {
      Talk = StrRight(Stuff, StrGetLength(Stuff) - 5);
      Talk = StrTrimLeft(Talk);
    }
    ReadLine();
  }
}

/***********************************************************
* Read a line from Mobile file                             *
************************************************************/

void Mobile::ReadLine()
{
  MobileFile.ReadString(Stuff);
  Stuff = StrTrimLeft(Stuff);
  Stuff = StrTrimRight(Stuff);
}