/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   World.cpp                                        *
* Usage:  Functions to make the world come alive.          *
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
#include "World.h"

/***********************************************************
* Globals                                                  *
************************************************************/

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Create 'spawn mobile' events                             *
************************************************************/

void World::CreateSpawnMobileEvents()
{
  CStdioFile ControlMobSpawnFile;
  CString    ControlMobSpawnFileName;
  int        Count;
  int        CurrentTime;
  int        Days;
  CStdioFile EventFile;
  CString    EventFileName;
  CString    EventTime;
  CFileFind  FileList;
  int        Hours;
  int        Limit;
  int        Minutes;
  CString    MobileId;
  int        Months;
  BOOL       MoreFiles;
  CString    RoomId;
  int        Seconds;
  CString    Stuff;
  int        Success;
  CString    TmpStr;
  int        Weeks;
  CStdioFile WorldMobileFile;
  CString    WorldMobileFileName;
  int        Years;

  if (ChgDir(WORLD_MOBILES_DIR))
  { // Change directory failed
    AfxMessageBox("World::CreateSpawnMobileEvents - Change directory to WORLD_MOBILES_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::CreateSpawnMobileEvents - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    WorldMobileFileName = FileList.GetFileName();
    MobileId = WorldMobileFileName.Left(WorldMobileFileName.GetLength()-4);
    if (MobileId == "ReadMe")
    {
      continue;
    }
    //* Have we already created a spawn event for this MobileId?
    ControlMobSpawnFileName =  CONTROL_MOB_SPAWN_DIR;
    ControlMobSpawnFileName += MobileId;
    Success = ControlMobSpawnFile.Open(ControlMobSpawnFileName,
                            CFile::modeRead |
                            CFile::typeText);
    if(Success)
    { // The NoMoreSpawnEventsFlag is set for this mobile
      ControlMobSpawnFile.Close();
      continue;
    }
    //* Check MaxInWorld against actual 'in world' count
    WorldMobileFileName = WORLD_MOBILES_DIR + WorldMobileFileName;
    Success = WorldMobileFile.Open(WorldMobileFileName,
                        CFile::modeRead |
                        CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("World::CreateSpawnMobileEvents - Open World Mobile file failed", MB_ICONSTOP);
      _endthread();
    }
    WorldMobileFile.ReadString(Stuff);
    if (GetWord(Stuff, 1) != "MaxInWorld:")
    { // World mobile file format error MaxInWorld
      AfxMessageBox("World::CreateSpawnMobileEvents - World mobile file format error MaxInWorld", MB_ICONSTOP);
      _endthread();
    }
    Count    = CountMob(MobileId);
    Limit    = atoi(GetWord(Stuff,2));
    if (Count >= Limit)
    { // No spawn event needed
      WorldMobileFile.Close();
      continue;
    }
    //*******************************
    //* Create 'spawn mobile' event *
    //*******************************
    WorldMobileFile.ReadString(Stuff);
    if (GetWord(Stuff, 1) != "RoomId:")
    { // World mobile file format error RoomId
      AfxMessageBox("World::CreateSpawnMobileEvents - World mobile file format error RoomId", MB_ICONSTOP);
      _endthread();
    }
    RoomId = GetWord(Stuff, 2);
    WorldMobileFile.ReadString(Stuff);
    if (GetWord(Stuff, 1) != "Interval:")
    { // World mobile file format error Interval
      AfxMessageBox("World::CreateSpawnMobileEvents - World mobile file format error Interval", MB_ICONSTOP);
      _endthread();
    }
    Seconds = atoi(GetWord(Stuff, 2)) * 1;
    Minutes = atoi(GetWord(Stuff, 3)) * 60;
    Hours   = atoi(GetWord(Stuff, 4)) * 3600;
    Days    = atoi(GetWord(Stuff, 5)) * 86400;
    Weeks   = atoi(GetWord(Stuff, 6)) * 604800;
    Months  = atoi(GetWord(Stuff, 7)) * 2592000;
    Years   = atoi(GetWord(Stuff, 8)) * 31104000;
    CurrentTime = GetTimeSeconds();
    CurrentTime += Seconds;
    CurrentTime += Minutes;
    CurrentTime += Hours;
    CurrentTime += Days;
    CurrentTime += Weeks;
    CurrentTime += Months;
    CurrentTime += Years;
    sprintf(Buf, "%d", CurrentTime);
    EventTime = ConvertStringToCString(Buf);
    EventFileName =  CONTROL_EVENTS_DIR;
    EventFileName += "M";
    EventFileName += EventTime;
    EventFileName += ".txt";
    Success = EventFile.Open(EventFileName,
                  CFile::modeRead |
                  CFile::typeText);
    if(Success)
    { // Event file already exists
      EventFile.Close();
      Success = EventFile.Open(EventFileName,
                    CFile::modeReadWrite |
                    CFile::typeText);
      if(!Success)
      { // Open for write failed
        AfxMessageBox("World::CreateSpawnMobileEvents - Open Events file failed - write", MB_ICONSTOP);
        _endthread();
      }
      EventFile.ReadString(Stuff);
      while (Stuff != "")
      {
        EventFile.ReadString(Stuff);
      }
    }
    else
    { // New event file
      Success = EventFile.Open(EventFileName,
                    CFile::modeCreate |
                    CFile::modeWrite  |
                    CFile::typeText);
      if(!Success)
      { // Open for write failed
        AfxMessageBox("World::CreateSpawnMobileEvents - Open Events file failed - create", MB_ICONSTOP);
        _endthread();
      }
    }
    while (Count < Limit)
    {
      TmpStr =  MobileId;
      TmpStr += " ";
      TmpStr += RoomId;
      TmpStr += "\r\n";
      EventFile.WriteString(TmpStr);
      Count++;
    }
    EventFile.Close();
    WorldMobileFile.Close();
    // Set the NoMoreSpawnEventsFlag for this mobile
    Success = ControlMobSpawnFile.Open(ControlMobSpawnFileName,
                            CFile::modeCreate |
                            CFile::modeWrite  |
                            CFile::typeText);
    if(!Success)
    { // Create file failed
      AfxMessageBox("World::CreateSpawnMobileEvents - Create Control Mobile Spawn file failed", MB_ICONSTOP);
      _endthread();
    }
    ControlMobSpawnFile.Close();
  }
}

/***********************************************************
* Check 'spawn mobile' events                              *
************************************************************/

void World::CheckSpawnMobileEvents()
{
  CString    CheckTime;
  CString    ControlMobSpawnFileName;
  CStdioFile EventFile;
  CString    EventFileName;
  CString    EventTime;
  CFileFind  FileList;
  CString    MobileId;
  BOOL       MoreFiles;
  CString    RoomId;
  CString    Stuff;
  int        Success;

  sprintf(Buf, "%d", GetTimeSeconds());
  CheckTime = ConvertStringToCString(Buf);
  if (ChgDir(CONTROL_EVENTS_DIR))
  { // Change directory failed
    AfxMessageBox("World::CheckSpawnMobileEvents - Change directory to CONTROL_EVENTS_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Event files starting with 'M' are 'spawn mobile' events
  MoreFiles = FileList.FindFile("M*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::CheckSpawnMobileEvents - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  {
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    // Is it time for this event
    EventFileName += FileList.GetFileName();
    EventTime = EventFileName.Left(EventFileName.GetLength()-4);
    EventTime = EventTime.Right(EventTime.GetLength()-1);
    if (EventTime > CheckTime)
    { // Event is in the future, so skip it
      continue;
    }
    // Event's time has arrived
    EventFileName = CONTROL_EVENTS_DIR + EventFileName;
    Success = EventFile.Open(EventFileName,
                  CFile::modeRead |
                  CFile::typeText);
    if(!Success)
    { // File does not exist - Very bad!
      AfxMessageBox("World::CheckSpawnMobileEvents - Open Events file failed", MB_ICONSTOP);
      _endthread();
    }
    EventFile.ReadString(Stuff);
    while (Stuff != "")
    { // Get RoomId, MobileId, then spawn the mob
      MobileId = GetWord(Stuff, 1);
      RoomId   = GetWord(Stuff, 2);
      SpawnMobile(MobileId, RoomId);
      // Remove the NoMoreSpawnEventsFlag for this mobile
      // This is overkill, attempts to remove same flag over and over
      ControlMobSpawnFileName =  CONTROL_MOB_SPAWN_DIR;
      ControlMobSpawnFileName += MobileId;
      TRY
      {
        CFile::Remove(ControlMobSpawnFileName);
      }
      CATCH (CFileException, e)
      { // Don't care if file already removed
      }
      END_CATCH
      EventFile.ReadString(Stuff);
    }
    // Event completed, remove it
    EventFile.Close();
    CFile::Remove(EventFileName);
  }
}

/***********************************************************
* Handle world events                                      *
************************************************************/

void World::Events()
{
  CreateSpawnMobileEvents();
  CheckSpawnMobileEvents();
  MakeMobilesMove();
}

/***********************************************************
* Heal mobiles                                             *
************************************************************/

void World::HealMobiles()
{
  CFileFind  FileList;
  bool       MobFighting;
  CString    MobileId;
  CString    MobNbr;
  CStdioFile MobStatsHitPointsFile;
  CString    MobStatsHitPointsFileName;
  BOOL       MoreFiles;
  int        PositionOfDot;
  CString    RoomId;
  CString    Stuff;

  if (ChgDir(MOB_STATS_HPT_DIR))
  { // Change directory failed
    AfxMessageBox("World::HealMobiles - Change directory to MOB_STATS_HPT_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get a list of all MobStats\HitPoints files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::HealMobiles - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  //*************************
  //* Heal no-fighting mobs *
  //*************************
  while (MoreFiles)
  { // For each MobStats\HitPoints file
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    MobStatsHitPointsFileName = FileList.GetFileName();
    MobileId = MobStatsHitPointsFileName.Left(MobStatsHitPointsFileName.GetLength()-4);
    MobFighting = HealMobilesFightCheck("MobPlayer", MobileId);
    if (MobFighting)
    { // Mobile is fighting, no heal
      continue;
    }
    MobFighting = HealMobilesFightCheck("PlayerMob", MobileId);
    if (MobFighting)
    { // Mobile is fighting, no heal
      continue;
    }
    //*******************
    //* Heal the mobile *
    //*******************
    RoomId = GetMobileRoom(MobileId);
    RemoveMobFromRoom(RoomId, MobileId);
    DeleteMobStats(MobileId);
    PositionOfDot = MobileId.Find('.');
    MobileId = MobileId.Left(PositionOfDot);
    AddMobToRoom(RoomId, MobileId);
  }
}

/***********************************************************
* See if mobile is fighting                                *
************************************************************/

bool World::HealMobilesFightCheck(CString Dir, CString MobileId)
{
  CFileFind  FileList;
  CStdioFile MobPlayerFile;
  CString    MobPlayerFileName;
  bool       MobFighting;
  BOOL       MoreFiles;
  CString    Stuff;
  int        Success;
  CString    TmpStr;

  MobFighting = false;
  if (Dir == "MobPlayer")
  { // Checking MobPlayer
    if (ChgDir(MOB_PLAYER_DIR))
    { // Change directory failed
      AfxMessageBox("World::HealMobilesFightCheck - Change directory to MOB_PLAYER_DIR failed", MB_ICONSTOP);
      _endthread();
    }
  }
  if (Dir == "PlayerMob")
  { // Checking PlayerMob
    if (ChgDir(PLAYER_MOB_DIR))
    { // Change directory failed
      AfxMessageBox("World::HealMobilesFightCheck - Change directory to PLAYER_MOB_DIR failed", MB_ICONSTOP);
      _endthread();
    }
  }
  // Get a list of all MobPlayer files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::HealMobilesFightCheck - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  while (MoreFiles)
  { // For each MobPlayer or PlayerMob file
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    MobPlayerFileName = FileList.GetFileName();
    // Set file name based on Dir
    if (Dir == "MobPlayer")
    { // Checking MobPlayer
      MobPlayerFileName =  MOB_PLAYER_DIR + MobPlayerFileName;
    }
    if (Dir == "PlayerMob")
    { // Checking PlayerMob
      MobPlayerFileName =  PLAYER_MOB_DIR + MobPlayerFileName;
    }
    Success = MobPlayerFile.Open(MobPlayerFileName,
                      CFile::modeRead |
                      CFile::typeText);
    if(!Success)
    { // Failed to open MobPlayer or MobPlayer file
      TmpStr  = "World::HealMobilesFightCheck - Open ";
      TmpStr += Dir;
      TmpStr += " file failed";
      AfxMessageBox(TmpStr, MB_ICONSTOP);
      _endthread();
    }
    MobPlayerFile.ReadString(Stuff);
    while (Stuff != "")
    { // Read all lines
      if (Stuff == MobileId)
      { // A match means the mobile is fighting
        MobFighting = true; 
      }
      MobPlayerFile.ReadString(Stuff);
    }
    MobPlayerFile.Close();
  }
  return MobFighting;
}

/***********************************************************
* Yep, believe it or not, this makes the mobs move         *
************************************************************/

void World::MakeMobilesMove()
{
  CStdioFile RoomMobListFile;
  CString    RoomMobListFileName;
  int        RoomMobListFileSize;
  CStdioFile RoomMobMoveFile;
  CString    RoomMobMoveFileName;
  int        RoomMobMoveFileSize;
  int        Success1;
  int        Success2;

  //********************************
  //* Check for existance of files *
  //********************************
  RoomMobListFileName  = CONTROL_DIR;
  RoomMobListFileName += "RoomMobList.txt";
  Success1 = RoomMobListFile.Open(RoomMobListFileName,
                       CFile::modeRead |
                       CFile::typeText);

  RoomMobMoveFileName  = CONTROL_DIR;
  RoomMobMoveFileName += "RoomMobMove.txt";
  Success2 = RoomMobMoveFile.Open(RoomMobMoveFileName,
                       CFile::modeRead |
                       CFile::typeText);
  if (Success1)
  { // RoomMobList file exists, but is it empty?
    RoomMobListFileSize = (int) RoomMobListFile.GetLength();
    RoomMobListFile.Close();
    if (RoomMobListFileSize == 0)
    { // Nothing in the MobList file
      Success1 = 0;
      TRY
      { // Delete it
        CFile::Remove(RoomMobListFileName);
      }
      CATCH (CFileException, e)
      { // If file remove fails, something is bad wrong!
        AfxMessageBox("World::MakeMobilesMove - Remove RoomMobList file failed", MB_ICONSTOP);
        _endthread();
      }
      END_CATCH
    }
  }
  if (Success2)
  { // RoomMobMove file exists, but is it empty?
    RoomMobMoveFileSize = (int) RoomMobMoveFile.GetLength();
    RoomMobMoveFile.Close();
    if (RoomMobMoveFileSize == 0)
    { // Nothing in the MobMove file
      Success2 = 0;
      TRY
      { // Delete it
        CFile::Remove(RoomMobMoveFileName);
      }
      CATCH (CFileException, e)
      { // If file remove fails, something is bad wrong!
        AfxMessageBox("World::MakeMobilesMove - Remove RoomMobMove file failed", MB_ICONSTOP);
        _endthread();
      }
      END_CATCH
    }
  }
  //***********************************
  //* Determine which file to process *
  //***********************************
  if (Success2)
  { // Process RoomMobMove until empty
    MakeMobilesMove3();
    return;
  }
  if (Success1)
  { // Process RoomMobList until empty, creates RoomMoveMove
    MakeMobilesMove2();
    return;
  }
  // Create RoomMobList,neither RoomMobList or RoomMobMove exist
  MakeMobilesMove1();
}

/***********************************************************
* Build file containing RoomMob file list                  *
************************************************************/

void World::MakeMobilesMove1()
{
  CFileFind    FileList;
  BOOL         MoreFiles;
  CStringArray RoomMobList;
  CString      RoomMobFileName;
  CStdioFile   RoomMobListFile;
  CString      RoomMobListFileName;
  int          RoomMobListFileSize;
  int          Success;
  CString      TmpStr;
  int          x;
  int          y;

  // Open MakeMobList file
  RoomMobListFileName =  CONTROL_DIR;
  RoomMobListFileName += "RoomMobList.txt";
  Success = RoomMobListFile.Open(RoomMobListFileName,
                      CFile::modeCreate |
                      CFile::modeWrite  |
                      CFile::typeText);
  if(!Success)
  { // Failed to open RoomMobMove file
    AfxMessageBox("World::MakeMobilesMove1 - Create RoomMobList file failed", MB_ICONSTOP);
    _endthread();
  }
  if (ChgDir(ROOM_MOB_DIR))
  { // Change directory failed
    AfxMessageBox("World::MakeMobilesMove1 - Change directory to ROOM_MOB_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  // Get a list of all RoomMob files
  MoreFiles = FileList.FindFile("*.*");
  if (ChgDir(HomeDir))
  { // Change to home directory failed
    AfxMessageBox("World::MakeMobilesMove1 - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  //***************************
  //* Create RoomMobList file *
  //***************************
  while (MoreFiles)
  { // Process all rooms that have mobiles in them
    MoreFiles = FileList.FindNextFile();
    if (FileList.IsDirectory())
    { // Skip directories
      continue;
    }
    RoomMobFileName = FileList.GetFileName();
    if (RoomMobFileName.Find("Spawn") == -1)
    { // Not a spawn room, Random position in list
      sprintf(Buf, "%05d",rand());
      TmpStr = ConvertStringToCString(Buf);
    }
    else
    { // Force 'spawn' rooms to be first in list
      TmpStr = "00000";
    }
    TmpStr += " ";
    TmpStr += RoomMobFileName;
    RoomMobList.Add(TmpStr);
  }
  // sort em
  y = RoomMobList.GetUpperBound();
  int i, j;
  i = 0;
  while( i < (y - 1) )
  {
    j = i + 1;
    while( j < y )
    {
      if( RoomMobList.GetAt(i) > RoomMobList.GetAt(j) )
      {
        TmpStr = RoomMobList.GetAt(i);
        RoomMobList.SetAt(i, RoomMobList.GetAt(j));
        RoomMobList.SetAt(j, TmpStr);
      }
      j++;
    }
    i++;
  }
  // Write em
  for (x = 0; x <= y ; x++)
  { // For each string in the ValidCmds CStringArray
    TmpStr = RoomMobList.GetAt(x);
    TmpStr = GetWord(TmpStr, 2);
    TmpStr += "\n";
    RoomMobListFile.WriteString(TmpStr);
  }
  RoomMobListFileSize = (int) RoomMobListFile.GetLength();
  RoomMobListFile.Close();
  if (RoomMobListFileSize == 0)
  { // No mobiles are moving, MobMove file is empty
    TRY
    { // Delete it
      CFile::Remove(RoomMobListFileName);
    }
    CATCH (CFileException, e)
    { // If file remove fails, something is bad wrong!
      AfxMessageBox("World::MakeMobilesMove1 - Remove RoomMobList file failed", MB_ICONSTOP);
      _endthread();
    }
    END_CATCH
  }
}

/***********************************************************
* Build file containing mobiles to be moved                *
************************************************************/

void World::MakeMobilesMove2()
{
  CStdioFile ControlMobNoMoveFile;
  CString    ControlMobNoMoveFileName;
  int        ExitCount;
  int        ExitNumber;
  CString    ExitToRoomId;
  CFileFind  FileList;
  int        i;
  int        MobCount;
  CString    MobileId;
  CString    MobileIdCheck;
  bool       MobListNotCompleted = false;
  int        PositionOfDot;
  int        RandomPct;
  CString    RoomId;
  string     sRoomId;
  CStdioFile RoomMobFile;
  CString    RoomMobFileName;
  CStdioFile RoomMobListFile;
  CString    RoomMobListFileName;
  CStdioFile RoomMobListTempFile;
  CString    RoomMobListTempFileName;
  CStdioFile RoomMobMoveFile;
  CString    RoomMobMoveFileName;
  CString    Stuff;
  int        Success;
  clock_t    TimerStart;
  clock_t    TimerStop;
  CString    TmpStr;
  CString    ValidMobRoomExits;
  string     sValidMobRoomExits;

  // Open MakeMobList file
  RoomMobListFileName  = CONTROL_DIR;
  RoomMobListFileName += "RoomMobList.txt";
  Success = RoomMobListFile.Open(RoomMobListFileName,
                      CFile::modeRead |
                      CFile::typeText);
  if(!Success)
  { // Failed to open RoomMobList file
    AfxMessageBox("World::MakeMobilesMove1 - Create RoomMobList file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open MakeMobListTemp file
  RoomMobListTempFileName  = CONTROL_DIR;
  RoomMobListTempFileName += "RoomMobListTemp.txt";
  Success = RoomMobListTempFile.Open(RoomMobListTempFileName,
                          CFile::modeCreate |
                          CFile::modeWrite  |
                          CFile::typeText);
  if(!Success)
  { // Failed to open RoomMobListTemp file
    AfxMessageBox("World::MakeMobilesMove2 - Create RoomMobListTemp file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open RoomMobMove file
  RoomMobMoveFileName  = CONTROL_DIR;
  RoomMobMoveFileName += "RoomMobMove.txt";
  Success = RoomMobMoveFile.Open(RoomMobMoveFileName,
                      CFile::modeCreate |
                      CFile::modeWrite  |
                      CFile::typeText);
  if(!Success)
  { // Failed to open RoomMobMove file
    AfxMessageBox("World::MakeMobilesMove2 - Create RoomMobMove file failed", MB_ICONSTOP);
    _endthread();
  }
  //***************************
  //* Create RoomMobMove file *
  //***************************
  TimerStart = clock();
  TimerStop  = TimerStart + 100;
  RoomMobListFile.ReadString(RoomMobFileName);
  while (RoomMobFileName != "")
  { // Process all rooms that have mobiles in them
    if (clock() > TimerStop)
    { // Time to stop so cpu is not maxed
      MobListNotCompleted = true;
      TmpStr  = RoomMobFileName;
      TmpStr += "\n";
      RoomMobListTempFile.WriteString(TmpStr);
      RoomMobListFile.ReadString(RoomMobFileName);
      continue;
    }
    RoomId = RoomMobFileName.Left(RoomMobFileName.GetLength()-4);
    // Open RoomMob file
    RoomMobFileName =  ROOM_MOB_DIR + RoomMobFileName;
    Success = RoomMobFile.Open(RoomMobFileName,
                    CFile::modeRead |
                    CFile::typeText);
    if(!Success)
    { // No RoomMob file? Really, I guess all the mobs got themselves killed
      RoomMobListFile.ReadString(RoomMobFileName);
      continue;
    }
    RoomMobFile.ReadString(Stuff);
    while (Stuff != "")
    { // For each mobile in room
      MobCount = atoi(GetWord(Stuff, 1));
      MobileId = GetWord(Stuff, 2);
      MobileIdCheck = MobileId;
      PositionOfDot = MobileIdCheck.Find('.');
      if (PositionOfDot > 1)
      { // Mobile is hurt but not fighting
        MobileIdCheck = MobileIdCheck.Left(PositionOfDot);
      }
      //* Is the MobNoMoveFlag set?
      ControlMobNoMoveFileName =  CONTROL_MOB_NOMOVE_DIR;
      ControlMobNoMoveFileName += MobileIdCheck;
      Success = ControlMobNoMoveFile.Open(ControlMobNoMoveFileName,
                               CFile::modeRead |
                               CFile::typeText);
      if(Success)
      { // The MobNoMoveFlag is set for this mobile
        ControlMobNoMoveFile.Close();
      }
      else
      { // Mobile may move
        for (i = 1; i <= MobCount; i++)
        { // For each mobile occurrence
          if (RoomId.Find("Spawn") == -1)
          { // Not a spawn room, Get random chance of mob moving
            RandomPct = GetRandomNumber(100);
          }
          else
          { // Force mobs in 'spawn' rooms to move
            RandomPct = -1;
          }
          if (RandomPct <= MOB_MOVE_PCT)
          { // Mobile is to be moved
            sRoomId = ConvertCStringToString(RoomId);
            sValidMobRoomExits = GetValidMobRoomExits(sRoomId);;
            ValidMobRoomExits = ConvertStringToCString(sValidMobRoomExits);
            ExitCount         = WordCount(ValidMobRoomExits);
            if (ExitCount > 0)
            { // Mob has at least one exit available
              ExitNumber        = GetRandomNumber(ExitCount);
              ExitToRoomId      = GetWord(ValidMobRoomExits, ExitNumber);
              if (ExitToRoomId == "")
              { //  Blow up for now, but we should LogThis, not blow up??
                AfxMessageBox("ExitToRoomId is blank zz", MB_ICONSTOP);
                _endthread();
              }
              TmpStr  = MobileId;
              TmpStr += " ";
              TmpStr += RoomId;
              TmpStr += " ";
              TmpStr += ExitToRoomId;
              TmpStr += "\n";
              RoomMobMoveFile.WriteString(TmpStr);
            }
          }
        }
      }
      RoomMobFile.ReadString(Stuff);
    }
    RoomMobFile.Close();
    RoomMobListFile.ReadString(RoomMobFileName);
  }
  // Close files
  RoomMobMoveFile.Close();
  RoomMobListFile.Close();
  RoomMobListTempFile.Close();
  TRY
  { // Done with RoomMobList file, get rid of it
    CFile::Remove(RoomMobListFileName);
  }
  CATCH (CFileException, e)
  { // If file remove fails, something is bad wrong!
    AfxMessageBox("World::MakeMobilesMove2 - Remove RoomMobList file failed", MB_ICONSTOP);
    _endthread();
  }
  END_CATCH
  if (MobListNotCompleted)
  { // Time ran out before MobList was completely processed
    TRY
    { // Rename temp file
      CFile::Rename(RoomMobListTempFileName, RoomMobListFileName);
    }
    CATCH (CFileException, e)
    { // If rename fails, something is bad wrong!
      AfxMessageBox("World::MakeMobilesMove2 - Rename RoomMobListTemp file failed", MB_ICONSTOP);
      _endthread();
    }
    END_CATCH
  }
  else
  { // MobList was completely processed
    TRY
    { // delete temp file
      CFile::Remove(RoomMobListTempFileName);
    }
    CATCH (CFileException, e)
    { // If delete fails, something is bad wrong!
      AfxMessageBox("World::MakeMobilesMove2 - Rename RoomMobListTemp file failed", MB_ICONSTOP);
      _endthread();
    }
    END_CATCH
  }
}

/***********************************************************
* Ok, now we really are going to move the mobiles          *
************************************************************/

void World::MakeMobilesMove3()
{
  CString    ArriveMsg;
  CString    ExitToRoomId;
  CString    LeaveMsg;
  CString    MobileDesc1;
  CString    MobileId;
  bool       MobMoveNotCompleted;
  CString    MobStatsFileName;
  int        PositionOfDot;
  CString    RoomId;
  CStdioFile RoomMobMoveFile;
  CString    RoomMobMoveFileName;
  CStdioFile RoomMobMoveTempFile;
  CString    RoomMobMoveTempFileName;
  CString    Stuff;
  int        Success;
  clock_t    TimerStart;
  clock_t    TimerStop;

  //******************************
  //* Initization and open files *
  //******************************
  MobMoveNotCompleted = false;
  RoomMobMoveFileName =  CONTROL_DIR;
  RoomMobMoveFileName += "RoomMobMove.txt";
  Success = RoomMobMoveFile.Open(RoomMobMoveFileName,
                      CFile::modeRead |
                      CFile::typeText);
  if(!Success)
  { // No RoomMobMove file, Ok, who delete the file when I wasn't looking?
    AfxMessageBox("World::MakeMobilesMove3 - Open RoomMobMove failed", MB_ICONSTOP);
    _endthread();
  }
  RoomMobMoveTempFileName =  CONTROL_DIR;
  RoomMobMoveTempFileName += "RoomMobMoveTemp.txt";
  Success = RoomMobMoveTempFile.Open(RoomMobMoveTempFileName,
                          CFile::modeCreate |
                          CFile::modeWrite  |
                          CFile::typeText);
  if(!Success)
  { // RoomMobMoveTemp file failed to open
    AfxMessageBox("World::MakeMobilesMove3 - Open RoomMobMoveTemp failed", MB_ICONSTOP);
    _endthread();
  }
  //****************************
  //* Process RoomMobMove file *
  //****************************
  TimerStart = clock();
  TimerStop  = TimerStart + 100;
  RoomMobMoveFile.ReadString(Stuff);
  while (Stuff != "")
  { // For each mob to be moved
    if (clock() > TimerStop)
    { // Time to stop so cpu is not maxed
      MobMoveNotCompleted = true;
      Stuff += "\n";
      RoomMobMoveTempFile.WriteString(Stuff);
      RoomMobMoveFile.ReadString(Stuff);
      continue;
    }
    MobileId     = GetWord(Stuff, 1);
    RoomId       = GetWord(Stuff, 2);
    ExitToRoomId = GetWord(Stuff, 3);
    if (!IsMobileIdInRoom(RoomId, MobileId))
    { // Mob not in room anymore, prolly get itself killed, so can't be moved
      RoomMobMoveFile.ReadString(Stuff);
      continue;
    }
    MobileDesc1  = GetMobDesc1(MobileId);
    LeaveMsg     = MobileDesc1;
    LeaveMsg    += " leaves.";
    ArriveMsg    = MobileDesc1;
    ArriveMsg   += " arrives.";
    RemoveMobFromRoom(RoomId, MobileId);
    AddMobToRoom(ExitToRoomId, MobileId);
    pDnodeSrc = NULL;
    pDnodeTgt = NULL;
    SendToRoom(RoomId,       LeaveMsg);
    SendToRoom(ExitToRoomId, ArriveMsg);
    PositionOfDot = MobileId.Find('.');
    if (PositionOfDot > 1)
    { // Delete 'MobStats' Room file
      MobStatsFileName = MOB_STATS_ROOM_DIR;
      MobStatsFileName += MobileId;
      MobStatsFileName += ".txt";
      TRY
      {
        CFile::Remove(MobStatsFileName);
      }
      CATCH (CFileException, e)
      { // If file remove fails, something is bad wrong!
        AfxMessageBox("World::MakeMobilesMove - Remove MobStats Room file failed", MB_ICONSTOP);
        _endthread();
      }
      END_CATCH
      // Write new RoomId into MobStats Room file
      CreateMobStatsFileWrite(MOB_STATS_ROOM_DIR, MobileId, ExitToRoomId);
    }
    // Read next line
    RoomMobMoveFile.ReadString(Stuff);
  }
  // Close RoomMobMove files
  RoomMobMoveFile.Close();
  RoomMobMoveTempFile.Close();
  // Done with RoomMobMove file, get rid of it
  TRY
  {
    CFile::Remove(RoomMobMoveFileName);
  }
  CATCH (CFileException, e)
  { // If file remove fails, something is bad wrong!
    AfxMessageBox("World::MakeMobilesMove3 - Remove RoomMobMove file failed", MB_ICONSTOP);
    _endthread();
  }
  END_CATCH
  // Check whether or not mobs got moved
  if (MobMoveNotCompleted)
  { // Time ran out before all the mobs got moved, so moved the rest of them later
    TRY
    {
      CFile::Rename(RoomMobMoveTempFileName, RoomMobMoveFileName);
    }
    CATCH (CFileException, e)
    { // If file rename fails, something is bad wrong!
      AfxMessageBox("World::MakeMobilesMove3 - Rename RoomMobMoveTemp file failed", MB_ICONSTOP);
      _endthread();
    }
    END_CATCH
  }
  else
  { // All mobs got moved, delete the temp file
    TRY
    {
      CFile::Remove(RoomMobMoveTempFileName);
    }
    CATCH (CFileException, e)
    { // If file rename fails, something is bad wrong!
      AfxMessageBox("World::MakeMobilesMove3 - Rename RoomMobMoveTemp file failed", MB_ICONSTOP);
      _endthread();
    }
    END_CATCH
  }
}

/***********************************************************
* Osi - OMugs script interpreter                           *
************************************************************/

void World::Osi(CString ScriptType, CString ScriptId)
{
  Symbol      **vpSymbol; // Symbol vector -- an array of Symbol pointers
  Executor    *pExecutor;
  Icode       *pIcode;
  Parser      *pParser;
  Symbol      *pSymbolRoot;
  CFileStatus  FileStatus;
  int          SymbolCount;

  // Build script file name
  ScriptFileName  = SCRIPTS_DIR;
  ScriptFileName += ScriptType;
  ScriptFileName += "\\";
  ScriptFileName += ScriptId;
  ScriptFileName += ".txt";
  // Does the script file exist?
  if (!CFile::GetStatus(ScriptFileName.c_str(), FileStatus))
  { // No script file, nothing to do
    return;
  }
  // Parse script
  pParser = new Parser();
  pParser->Parse();

  // Declare an array of Symbol pointers
  SymbolCount = Symbol::GetSymbolCount();
  vpSymbol = new Symbol *[SymbolCount+1];

  // Convert symbol table
  pSymbolRoot = pParser->GetSymbolRootPtr();
  pSymbolRoot->ConvertSymbolTable(vpSymbol);

  // Get Icode pointer, set the symbol vector in Icode, and we are done with parser
  pIcode = pParser->GetIcodePtr();
  pIcode->SetSymbolVector(vpSymbol);
  delete pParser;

  // Execute the script
  pExecutor = new Executor(pSymbolRoot, pIcode);
  pExecutor->Execute();

  // Clean up
  delete[] vpSymbol;
  delete   pExecutor;
}

/***********************************************************
* Spawn a mobile so players have something to whack!       *
************************************************************/

void World::SpawnMobile(CString MobileId, CString RoomId)
{
  Mobile  *pMobile;
  CString  AfxMessage;
  CString  MobileAction;
  CString  SpawnMsg;

  //********************
  //* Spawn the mobile *
  //********************
  pMobile = Mobile::IsMobValid(MobileId);
  if (!pMobile)
  { // Very bad, no such mobile
    AfxMessage  = "World::SpawnMobile - Mobile not found.";
    AfxMessage += "\n";
    AfxMessage += "MobileId: ";
    AfxMessage += MobileId;
    AfxMessageBox(AfxMessage, MB_ICONSTOP);
    _endthread();
  }
  AddMobToRoom(RoomId, MobileId);
  SpawnMsg = pMobile->Desc1;
  SpawnMsg += " suddenly appears!";
  pDnodeSrc = NULL;
  pDnodeTgt = NULL;
  SendToRoom(RoomId, SpawnMsg);
  MobileAction = pMobile->Action;
  delete pMobile;
  if (IsWord("NoMove", MobileAction))
  {
    SpawnMobileNoMove(MobileId);
  }
}

/***********************************************************
* Make mobile stand still                                  *
************************************************************/

void World::SpawnMobileNoMove(CString MobileId)
{
  CStdioFile ControlMobNoMoveFile;
  CString    ControlMobNoMoveFileName;
  int        Success;

  ControlMobNoMoveFileName  = CONTROL_MOB_NOMOVE_DIR;
  ControlMobNoMoveFileName += MobileId;
  Success = ControlMobNoMoveFile.Open(ControlMobNoMoveFileName,
                           CFile::modeCreate |
                           CFile::modeWrite  |
                           CFile::typeText);
  if(!Success)
  { // Create file failed
    AfxMessageBox("World::SpawnMobile - Create Control Mobile NoMove file failed", MB_ICONSTOP);
    _endthread();
  }
  ControlMobNoMoveFile.Close();
}