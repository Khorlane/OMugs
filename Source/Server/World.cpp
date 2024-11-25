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

Mobile *IsMobValid(string MobileId); // Mobile

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Create 'spawn mobile' events                             *
************************************************************/

void World::CreateSpawnMobileEvents()
{
  ofstream   ControlMobSpawnFile;
  string     ControlMobSpawnFileName;
  int        Count;
  int        CurrentTime;
  int        Days;
  fstream    EventFile;
  string     EventFileName;
  string     EventTime;
  int        Hours;
  int        Limit;
  int        Minutes;
  string     MobileId;
  int        Months;
  string     RoomId;
  int        Seconds;
  string     Stuff;
  string     TmpStr;
  int        Weeks;
  ifstream   WorldMobileFile;
  string     WorldMobileFileName;
  int        Years;

  if (ChgDir(WORLD_MOBILES_DIR))
  { // Change directory failed
    AfxMessageBox("World::CreateSpawnMobileEvents - Change directory to WORLD_MOBILES_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    { // Skip directories
      continue;
    }
    WorldMobileFileName = entry.path().filename().string();
    MobileId = StrLeft(WorldMobileFileName, StrGetLength(WorldMobileFileName) - 4);
    if (MobileId == "ReadMe")
    {
      continue;
    }
    //* Have we already created a spawn event for this MobileId?
    ControlMobSpawnFileName =  CONTROL_MOB_SPAWN_DIR;
    ControlMobSpawnFileName += MobileId;
    ControlMobSpawnFile.open(ControlMobSpawnFileName);
    if(ControlMobSpawnFile.is_open())
    { // The NoMoreSpawnEventsFlag is set for this mobile
      ControlMobSpawnFile.close();
      continue;
    }
    //* Check MaxInWorld against actual 'in world' count
    WorldMobileFileName = WORLD_MOBILES_DIR + WorldMobileFileName;
    WorldMobileFile.open(WorldMobileFileName);
    if(!WorldMobileFile.is_open())
    { // File does not exist - Very bad!
      AfxMessageBox("World::CreateSpawnMobileEvents - Open World Mobile file failed", MB_ICONSTOP);
      _endthread();
    }
    getline(WorldMobileFile, Stuff);
    if (StrGetWord(Stuff, 1) != "MaxInWorld:")
    { // World mobile file format error MaxInWorld
      AfxMessageBox("World::CreateSpawnMobileEvents - World mobile file format error MaxInWorld", MB_ICONSTOP);
      _endthread();
    }
    Count    = CountMob(MobileId);
    Limit    = stoi(StrGetWord(Stuff,2));
    if (Count >= Limit)
    { // No spawn event needed
      WorldMobileFile.close();
      continue;
    }
    //*******************************
    //* Create 'spawn mobile' event *
    //*******************************
    getline(WorldMobileFile, Stuff);
    if (StrGetWord(Stuff, 1) != "RoomId:")
    { // World mobile file format error RoomId
      AfxMessageBox("World::CreateSpawnMobileEvents - World mobile file format error RoomId", MB_ICONSTOP);
      _endthread();
    }
    RoomId = StrGetWord(Stuff, 2);
    getline(WorldMobileFile, Stuff);
    if (StrGetWord(Stuff, 1) != "Interval:")
    { // World mobile file format error Interval
      AfxMessageBox("World::CreateSpawnMobileEvents - World mobile file format error Interval", MB_ICONSTOP);
      _endthread();
    }
    Seconds = stoi(StrGetWord(Stuff, 2)) * 1;
    Minutes = stoi(StrGetWord(Stuff, 3)) * 60;
    Hours   = stoi(StrGetWord(Stuff, 4)) * 3600;
    Days    = stoi(StrGetWord(Stuff, 5)) * 86400;
    Weeks   = stoi(StrGetWord(Stuff, 6)) * 604800;
    Months  = stoi(StrGetWord(Stuff, 7)) * 2592000;
    Years   = stoi(StrGetWord(Stuff, 8)) * 31104000;
    CurrentTime = GetTimeSeconds();
    CurrentTime += Seconds;
    CurrentTime += Minutes;
    CurrentTime += Hours;
    CurrentTime += Days;
    CurrentTime += Weeks;
    CurrentTime += Months;
    CurrentTime += Years;
    sprintf(Buf, "%d", CurrentTime);
    EventTime = Buf;
    EventFileName =  CONTROL_EVENTS_DIR;
    EventFileName += "M";
    EventFileName += EventTime;
    EventFileName += ".txt";
    EventFile.open(EventFileName, ios_base::app);
    if (!EventFile.is_open())
    { // Open for append failed
        AfxMessageBox("World::CreateSpawnMobileEvents - Open Events file failed - append", MB_ICONSTOP);
        _endthread();
    }
    while (Count < Limit)
    {
      TmpStr =  MobileId;
      TmpStr += " ";
      TmpStr += RoomId;
      TmpStr += "\r\n";
      EventFile << TmpStr << endl;
      Count++;
    }
    EventFile.close();
    WorldMobileFile.close();
    // Set the NoMoreSpawnEventsFlag for this mobile
    ControlMobSpawnFile.open(ControlMobSpawnFileName);
    if(!ControlMobSpawnFile.is_open())
    { // Create file failed
      AfxMessageBox("World::CreateSpawnMobileEvents - Create Control Mobile Spawn file failed", MB_ICONSTOP);
      _endthread();
    }
    ControlMobSpawnFile.close();
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::CreateSpawnMobileEvents - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Check 'spawn mobile' events                              *
************************************************************/

void World::CheckSpawnMobileEvents()
{
  string     CheckTime;
  string     ControlMobSpawnFileName;
  ifstream   EventFile;
  string     EventFileName;
  string     EventTime;
  string     MobileId;
  string     RoomId;
  string     Stuff;

  sprintf(Buf, "%d", GetTimeSeconds());
  CheckTime = ConvertStringToCString(Buf);
  if (ChgDir(CONTROL_EVENTS_DIR))
  { // Change directory failed
    AfxMessageBox("World::CheckSpawnMobileEvents - Change directory to CONTROL_EVENTS_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    { // Skip directories
      continue;
    }
    EventFileName = entry.path().filename().string();
    if (!EventFileName.starts_with('M'))
    { // Event files starting with 'M' are 'spawn mobile' events
      continue;
    }
    // Is it time for this event
    EventTime = StrLeft(EventFileName, StrGetLength(EventFileName) - 4);
    EventTime = StrRight(EventTime, StrGetLength(EventTime) - 1);
    if (EventTime > CheckTime)
    { // Event is in the future, so skip it
      continue;
    }
    // Event's time has arrived
    EventFileName = CONTROL_EVENTS_DIR + EventFileName;
    EventFile.open(EventFileName);
    if(!EventFile.is_open())
    { // File does not exist - Very bad!
      AfxMessageBox("World::CheckSpawnMobileEvents - Open Events file failed", MB_ICONSTOP);
      _endthread();
    }
    getline(EventFile, Stuff);
    while (Stuff != "")
    { // Get RoomId, MobileId, then spawn the mob
      MobileId = StrGetWord(Stuff, 1);
      RoomId   = StrGetWord(Stuff, 2);
      SpawnMobile(MobileId, RoomId);
      // Remove the NoMoreSpawnEventsFlag for this mobile
      // This is overkill, attempts to remove same flag over and over
      ControlMobSpawnFileName =  CONTROL_MOB_SPAWN_DIR;
      ControlMobSpawnFileName += MobileId;
      Remove(ControlMobSpawnFileName);
      getline(EventFile, Stuff);
    }
    // Event completed, remove it
    EventFile.close();
    Remove(EventFileName);
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::CheckSpawnMobileEvents - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
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
  bool       MobFighting;
  string     MobileId;
  string     MobNbr;
  ifstream   MobStatsHitPointsFile;
  string     MobStatsHitPointsFileName;
  int        PositionOfDot;
  string     RoomId;
  string     Stuff;

  if (ChgDir(MOB_STATS_HPT_DIR))
  { // Change directory failed
    AfxMessageBox("World::HealMobiles - Change directory to MOB_STATS_HPT_DIR failed", MB_ICONSTOP);
    _endthread();
  }
  //*************************
  //* Heal no-fighting mobs *
  //*************************
  // Get a list of all MobStats\HitPoints files
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    { // Skip directories
      continue;
    }
    MobStatsHitPointsFileName = entry.path().filename().string();
    MobileId = StrLeft(MobStatsHitPointsFileName, StrGetLength(MobStatsHitPointsFileName) - 4);
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
    PositionOfDot = StrFindFirstChar(MobileId, '.');
    MobileId = StrLeft(MobileId, PositionOfDot);
    AddMobToRoom(RoomId, MobileId);
  }
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::HealMobiles - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* See if mobile is fighting                                *
************************************************************/

bool World::HealMobilesFightCheck(string Dir, string MobileId)
{
  ifstream   MobPlayerFile;
  string     MobPlayerFileName;
  bool       MobFighting;
  string     Stuff;
  string     TmpStr;

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
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    { // Skip directories
      continue;
    }
    MobPlayerFileName = entry.path().filename().string();
    // Set file name based on Dir
    if (Dir == "MobPlayer")
    { // Checking MobPlayer
      MobPlayerFileName =  MOB_PLAYER_DIR + MobPlayerFileName;
    }
    if (Dir == "PlayerMob")
    { // Checking PlayerMob
      MobPlayerFileName =  PLAYER_MOB_DIR + MobPlayerFileName;
    }
    MobPlayerFile.open(MobPlayerFileName);
    if(!MobPlayerFile.is_open())
    { // Failed to open MobPlayer or MobPlayer file
      TmpStr  = "World::HealMobilesFightCheck - Open ";
      TmpStr += Dir;
      TmpStr += " file failed";
      AfxMessageBox(ConvertStringToCString(TmpStr), MB_ICONSTOP);
      _endthread();
    }
    getline(MobPlayerFile, Stuff);
    while (Stuff != "")
    { // Read all lines
      if (Stuff == MobileId)
      { // A match means the mobile is fighting
        MobFighting = true; 
      }
      getline(MobPlayerFile, Stuff);
    }
    MobPlayerFile.close();
  }  
  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("World::HealMobilesFightCheck - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  return MobFighting;
}

/***********************************************************
* Yep, believe it or not, this makes the mobs move         *
************************************************************/

void World::MakeMobilesMove()
{
  ifstream   RoomMobListFile;
  string     RoomMobListFileName;
  ifstream   RoomMobMoveFile;
  string     RoomMobMoveFileName;
  bool       Success1;
  bool       Success2;

  //********************************
  //* Check for existance of files *
  //********************************
  Success1 = false;
  Success2 = false;
  RoomMobListFileName  = CONTROL_DIR;
  RoomMobListFileName += "RoomMobList.txt";
  RoomMobListFile.open(RoomMobListFileName);
  if (RoomMobListFile.is_open())
  {
    Success1 = true;
  }
  RoomMobMoveFileName  = CONTROL_DIR;
  RoomMobMoveFileName += "RoomMobMove.txt";
  RoomMobMoveFile.open(RoomMobMoveFileName);
  if (RoomMobMoveFile.is_open())
  {
    Success2 = true;
  }
  if (Success1)
  { // RoomMobList file exists, but is it empty?
    if (RoomMobListFile.peek() == ifstream::traits_type::eof())
    { // Nothing in the MobList file
      Success1 = false;
      RoomMobListFile.close();
      Remove(RoomMobListFileName);
    }
  }
  if (Success2)
  { // RoomMobMove file exists, but is it empty?
    if (RoomMobMoveFile.peek() == ifstream::traits_type::eof())
    { // Nothing in the MobMove file
      Success2 = false;
      Remove(RoomMobMoveFileName);
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
  vector<string> RoomMobList;
  string         RoomMobFileName;
  ofstream       RoomMobListFile;
  string         RoomMobListFileName;
  string         TmpStr;

  // Open MakeMobList file
  RoomMobListFileName =  CONTROL_DIR;
  RoomMobListFileName += "RoomMobList.txt";
  RoomMobListFile.open(RoomMobListFileName);
  if(!RoomMobListFile.is_open())
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
  for (const auto &entry : fs::directory_iterator("./"))
  {
    if (entry.is_directory())
    { // Skip directories
      continue;
    }
    RoomMobFileName = entry.path().filename().string();
    if (StrFind(RoomMobFileName, "Spawn") == -1)
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
    RoomMobList.push_back(TmpStr);
  }
  // sort em
   sort(RoomMobList.begin(), RoomMobList.end());
  // Write em
  for (string item : RoomMobList)
  {
    TmpStr = ConvertStringToCString(item);
    TmpStr = StrGetWord(TmpStr, 2);
    TmpStr += "\n";
    RoomMobListFile << TmpStr << endl;
  }
  RoomMobListFile.close();
  if (RoomMobList.empty())
  { // No mobiles are moving, MobMove file is empty
    Remove(RoomMobListFileName);
  }
  if (ChgDir(HomeDir))
  { // Change to home directory failed
    AfxMessageBox("World::MakeMobilesMove1 - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
}

/***********************************************************
* Build file containing mobiles to be moved                *
************************************************************/

void World::MakeMobilesMove2()
{
  ifstream   ControlMobNoMoveFile;
  string     ControlMobNoMoveFileName;
  int        ExitCount;
  int        ExitNumber;
  string     ExitToRoomId;
  int        i;
  int        MobCount;
  string     MobileId;
  string     MobileIdCheck;
  bool       MobListNotCompleted = false;
  int        PositionOfDot;
  int        RandomPct;
  string     RoomId;
  ifstream   RoomMobFile;
  string     RoomMobFileName;
  ifstream   RoomMobListFile;
  string     RoomMobListFileName;
  ofstream   RoomMobListTempFile;
  string     RoomMobListTempFileName;
  ofstream   RoomMobMoveFile;
  string     RoomMobMoveFileName;
  string     Stuff;
  clock_t    TimerStart;
  clock_t    TimerStop;
  string     TmpStr;
  string     ValidMobRoomExits;

  // Open MakeMobList file
  RoomMobListFileName  = CONTROL_DIR;
  RoomMobListFileName += "RoomMobList.txt";
  RoomMobListFile.open(RoomMobListFileName);
  if (!RoomMobListFile.is_open())
  { // Failed to open RoomMobList file
    AfxMessageBox("World::MakeMobilesMove1 - Create RoomMobList file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open MakeMobListTemp file
  RoomMobListTempFileName  = CONTROL_DIR;
  RoomMobListTempFileName += "RoomMobListTemp.txt";
  RoomMobListTempFile.open(RoomMobListTempFileName);
  if (!RoomMobListTempFile.is_open())
  { // Failed to open RoomMobListTemp file
    AfxMessageBox("World::MakeMobilesMove2 - Create RoomMobListTemp file failed", MB_ICONSTOP);
    _endthread();
  }
  // Open RoomMobMove file
  RoomMobMoveFileName  = CONTROL_DIR;
  RoomMobMoveFileName += "RoomMobMove.txt";
  RoomMobMoveFile.open(RoomMobMoveFileName);
  if (!RoomMobMoveFile.is_open())
  { // Failed to open RoomMobMove file
    AfxMessageBox("World::MakeMobilesMove2 - Create RoomMobMove file failed", MB_ICONSTOP);
    _endthread();
  }
  //***************************
  //* Create RoomMobMove file *
  //***************************
  TimerStart = clock();
  TimerStop  = TimerStart + 100;
  getline(RoomMobListFile, RoomMobFileName);
  while (RoomMobFileName != "")
  { // Process all rooms that have mobiles in them
    if (clock() > TimerStop)
    { // Time to stop so cpu is not maxed
      MobListNotCompleted = true;
      TmpStr  = RoomMobFileName;
      TmpStr += "\n";
      RoomMobListTempFile << TmpStr << endl;
      getline(RoomMobListFile, RoomMobFileName);
      continue;
    }
    RoomId = StrLeft(RoomMobFileName, StrGetLength(RoomMobFileName) - 4);
    // Open RoomMob file
    RoomMobFileName =  ROOM_MOB_DIR + RoomMobFileName;
    RoomMobFile.open(RoomMobFileName);
    if (!RoomMobFile.is_open())
    { // No RoomMob file? Really, I guess all the mobs got themselves killed
      getline(RoomMobListFile, RoomMobFileName);
      continue;
    }
    getline(RoomMobFile, Stuff);
    while (Stuff != "")
    { // For each mobile in room
      MobCount = stoi(StrGetWord(Stuff, 1));
      MobileId = StrGetWord(Stuff, 2);
      MobileIdCheck = MobileId;
      PositionOfDot = StrFindFirstChar(MobileIdCheck, '.');
      if (PositionOfDot > 1)
      { // Mobile is hurt but not fighting
        MobileIdCheck = StrLeft(MobileIdCheck, PositionOfDot);
      }
      //* Is the MobNoMoveFlag set?
      ControlMobNoMoveFileName =  CONTROL_MOB_NOMOVE_DIR;
      ControlMobNoMoveFileName += MobileIdCheck;
      if (ControlMobNoMoveFile.is_open())
      { // The MobNoMoveFlag is set for this mobile
        ControlMobNoMoveFile.close();
      }
      else
      { // Mobile may move
        for (i = 1; i <= MobCount; i++)
        { // For each mobile occurrence
          if (StrFind(RoomId, "Spawn") == -1)
          { // Not a spawn room, Get random chance of mob moving
            RandomPct = GetRandomNumber(100);
          }
          else
          { // Force mobs in 'spawn' rooms to move
            RandomPct = -1;
          }
          if (RandomPct <= MOB_MOVE_PCT)
          { // Mobile is to be moved
            ValidMobRoomExits = GetValidMobRoomExits(RoomId);;
            ExitCount         = StrCountWords(ValidMobRoomExits);
            if (ExitCount > 0)
            { // Mob has at least one exit available
              ExitNumber        = GetRandomNumber(ExitCount);
              ExitToRoomId      = StrGetWord(ValidMobRoomExits, ExitNumber);
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
              RoomMobMoveFile << TmpStr << endl;
            }
          }
        }
      }
      getline(RoomMobFile, Stuff);
    }
    RoomMobFile.close();
    getline(RoomMobListFile, RoomMobFileName);
  }
  // Close files
  RoomMobMoveFile.close();
  RoomMobListFile.close();
  RoomMobListTempFile.close();
  TRY
  { // Done with RoomMobList file, get rid of it
    Remove(RoomMobListFileName);
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
      Rename(RoomMobListTempFileName, RoomMobListFileName);
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
      Remove(RoomMobListTempFileName);
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
  string     ArriveMsg;
  string     ExitToRoomId;
  string     LeaveMsg;
  string     MobileDesc1;
  string     MobileId;
  bool       MobMoveNotCompleted;
  string     MobStatsFileName;
  int        PositionOfDot;
  string     RoomId;
  ifstream   RoomMobMoveFile;
  string     RoomMobMoveFileName;
  ofstream   RoomMobMoveTempFile;
  string     RoomMobMoveTempFileName;
  string     Stuff;
  clock_t    TimerStart;
  clock_t    TimerStop;

  //******************************
  //* Initization and open files *
  //******************************
  MobMoveNotCompleted = false;
  RoomMobMoveFileName =  CONTROL_DIR;
  RoomMobMoveFileName += "RoomMobMove.txt";
  RoomMobMoveFile.open(RoomMobMoveFileName);
  if (!RoomMobMoveFile.is_open())
  { // No RoomMobMove file, Ok, who delete the file when I wasn't looking?
    AfxMessageBox("World::MakeMobilesMove3 - Open RoomMobMove failed", MB_ICONSTOP);
    _endthread();
  }
  RoomMobMoveTempFileName =  CONTROL_DIR;
  RoomMobMoveTempFileName += "RoomMobMoveTemp.txt";
  RoomMobMoveTempFile.open(RoomMobMoveTempFileName);
  if (!RoomMobMoveTempFile.is_open())
  { // RoomMobMoveTemp file failed to open
    AfxMessageBox("World::MakeMobilesMove3 - Open RoomMobMoveTemp failed", MB_ICONSTOP);
    _endthread();
  }
  //****************************
  //* Process RoomMobMove file *
  //****************************
  TimerStart = clock();
  TimerStop  = TimerStart + 100;
  getline(RoomMobMoveFile, Stuff);
  while (Stuff != "")
  { // For each mob to be moved
    if (clock() > TimerStop)
    { // Time to stop so cpu is not maxed
      MobMoveNotCompleted = true;
      Stuff += "\n";
      RoomMobMoveTempFile << Stuff << endl;
      getline(RoomMobMoveFile, Stuff);
      continue;
    }
    MobileId     = StrGetWord(Stuff, 1);
    RoomId       = StrGetWord(Stuff, 2);
    ExitToRoomId = StrGetWord(Stuff, 3);
    if (!IsMobileIdInRoom(ConvertStringToCString(RoomId), ConvertStringToCString(MobileId)))
    { // Mob not in room anymore, prolly get itself killed, so can't be moved
      getline(RoomMobMoveFile, Stuff);
      continue;
    }
    MobileDesc1  = GetMobDesc1(ConvertStringToCString(MobileId));
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
    PositionOfDot = StrFindFirstChar(MobileId, '.');
    if (PositionOfDot > 1)
    { // Delete 'MobStats' Room file
      MobStatsFileName = MOB_STATS_ROOM_DIR;
      MobStatsFileName += MobileId;
      MobStatsFileName += ".txt";
      TRY
      {
        Remove(MobStatsFileName);
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
    getline(RoomMobMoveFile, Stuff);
  }
  // Close RoomMobMove files
  RoomMobMoveFile.close();
  RoomMobMoveTempFile.close();
  // Done with RoomMobMove file, get rid of it
  TRY
  {
    Remove(RoomMobMoveFileName);
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
      Rename(RoomMobMoveTempFileName, RoomMobMoveFileName);
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
      Remove(RoomMobMoveTempFileName);
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

void World::Osi(string ScriptType, string ScriptId)
{
  Symbol     **vpSymbol; // Symbol vector -- an array of Symbol pointers
  Executor    *pExecutor;
  Icode       *pIcode;
  Parser      *pParser;
  Symbol      *pSymbolRoot;
  int          SymbolCount;
  ifstream     ScriptFile;

  // Build script file name
  ScriptFileName  = SCRIPTS_DIR;
  ScriptFileName += ScriptType;
  ScriptFileName += "\\";
  ScriptFileName += ScriptId;
  ScriptFileName += ".txt";
  // Does the script file exist?
  ScriptFile.open(ScriptFileName);
  if (!ScriptFile.is_open())
  { // No script file, nothing to do
    ScriptFile.close();
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

void World::SpawnMobile(string MobileId, string RoomId)
{
  Mobile  *pMobile;
  string   AfxMessage;
  string   MobileAction;
  string   SpawnMsg;

  //********************
  //* Spawn the mobile *
  //********************
  pMobile = IsMobValid(MobileId);
  if (!pMobile)
  { // Very bad, no such mobile
    AfxMessage  = "World::SpawnMobile - Mobile not found.";
    AfxMessage += "\n";
    AfxMessage += "MobileId: ";
    AfxMessage += MobileId;
    AfxMessageBox(ConvertStringToCString(AfxMessage), MB_ICONSTOP);
    _endthread();
  }
  AddMobToRoom(RoomId, MobileId);
  SpawnMsg = ConvertCStringToString(pMobile->Desc1);
  SpawnMsg += " suddenly appears!";
  pDnodeSrc = NULL;
  pDnodeTgt = NULL;
  SendToRoom(RoomId, SpawnMsg);
  MobileAction = pMobile->Action;
  delete pMobile;
  if (IsWord("NoMove", ConvertStringToCString(MobileAction)))
  {
    SpawnMobileNoMove(MobileId);
  }
}

/***********************************************************
* Make mobile stand still                                  *
************************************************************/

void World::SpawnMobileNoMove(string MobileId)
{
  ofstream   ControlMobNoMoveFile;
  string     ControlMobNoMoveFileName;

  ControlMobNoMoveFileName  = CONTROL_MOB_NOMOVE_DIR;
  ControlMobNoMoveFileName += MobileId;
  ControlMobNoMoveFile.open(ControlMobNoMoveFileName);
  if(!ControlMobNoMoveFile.is_open())
  { // Create file failed
    AfxMessageBox("World::SpawnMobile - Create Control Mobile NoMove file failed", MB_ICONSTOP);
    _endthread();
  }
  ControlMobNoMoveFile.close();
}