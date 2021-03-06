/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   BigDog.cpp                                       *
* Usage:  Starting point for all OMugs stuff.              *
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
#include "BigDog.h"

/***********************************************************
 * BigDog                                                  *
 ***********************************************************/

void BigDog()
{
  WhoIsOnline *pWhoIsOnline;
  int          EventTick;
  string       GoGoGoFileName;
  string       LogBuf;
  int          MobHealTick;
  int          ReturnCode;
  string       StopItFileName;
  int          WhoIsOnlineTick;

  if (ChgDir(HomeDir))
  { // Change directory failed
    AfxMessageBox("BigDog - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  // Set Go Stop, force Go status
  StopItFileName  = CONTROL_DIR;
  StopItFileName += "StopIt";
  GoGoGoFileName  = CONTROL_DIR;
  GoGoGoFileName += "GoGoGo";
  if (FileExist(StopItFileName))
  { // If StopIt file exists, Rename it to GoGoGo
    ReturnCode = Rename(StopItFileName, GoGoGoFileName);
    if (ReturnCode != 0)
    {
      printf("Rename of 'StopIt' to 'GoGoGo' failed!");
      printf("Hard Exit!");
      exit(1);
    }
  }
  // Log game startup
  OpenLogFile();
  LogBuf  = "OMugs version ";
  LogBuf += VERSION ;
  LogBuf += " has started";
  LogIt(LogBuf);
  LogBuf  = "Home directory is ";
  LogBuf += HomeDir;
  LogIt(LogBuf);
  // Initialize
  EventTick        = EVENT_TICK;    // Force first tick at startup
  MobHealTick      = 0;
  WhoIsOnlineTick  = 0;
  StateConnections = true;
  StateRunning     = true;
  StateStopping    = false;
  srand((unsigned)time(NULL)); // Seed random number generator
  PACMN = 1.0f / MAC * MDRP / 100.0f;
  // Validate library files
  ValErr = ValidateIt("All");
  if (ValErr)
  { // Validation failed
    LogBuf = "OMugs has stopped";
    LogIt(LogBuf);
    CloseLogFile();
    return;
  }
  // Validation was ok, so open port, init, play on
  SockOpenPort(PORT_NBR);
  InitDescriptor();
  pCalendar = new Calendar;
  while (StateRunning)
  { // Game runs until it is stopped
    Sleep(MILLI_SECONDS_TO_SLEEP);
    AdvanceTime();
    if (!StateStopping)
    { // Game is not stopping, but should it be?
      if (FileExist(StopItFileName))
      { // StopIt file was found, Stop the game
        StateStopping = true;
        LogBuf = "Game is stopping";
        LogIt(LogBuf);
      }
    }
    if (!StateStopping)
    { // No new connections after stop command
      SockCheckForNewConnections();
      if (StateConnections && GetCount() == 1)
      { // No players connected
        LogBuf = "No Connections - going to sleep";
        LogIt(LogBuf);
        StateConnections = false;
      }
    }
    if (StateConnections)
    { // One or more players are connected
      SockRecv();
      EventTick++;
      if (EventTick >= EVENT_TICK)
      { // Time to process events
        EventTick = 0;
        Events();
      }
      MobHealTick++;
      if (MobHealTick >= MOB_HEAL_TICK)
      { // Time to heal mobs
        MobHealTick = 0;
        HealMobiles();
      }
    }
    else
    { // No connections
      if (StateStopping)
      { // Game is stopping
        StateRunning = false;
      }
    }
    // Who is online?
    WhoIsOnlineTick++;
    if (WhoIsOnlineTick >= WHO_IS_ONLINE_TICK)
    { // Time to see who is online
      WhoIsOnlineTick = 0;
      pWhoIsOnline = new WhoIsOnline(HomeDir);
      delete pWhoIsOnline;
    }
  }
  // Game has stopped so clean up
  ClearDescriptor();
  SockClosePort(PORT_NBR);
  pWhoIsOnline = new WhoIsOnline(HomeDir);
  delete pWhoIsOnline;
  delete pCalendar;
  LogBuf = "OMugs has stopped";
  LogIt(LogBuf);
  CloseLogFile();
}

/***********************************************************
* Globals                                                  *
************************************************************/

void AppTestCode()
{
  // Put test code here. It will be executed before dialogs are presented.
}

//
// File Functions
//

bool ChgDir(string Dir)
{
  return _chdir(Dir.c_str());
}

bool FileExist(string Name)
{
  bool Exist;

  ifstream f(Name.c_str());
  if (f.is_open())
  {
    Exist = true;
    f.close();
  }
  else
  {
    Exist = false;
  }
  return Exist;
}

int Rename(string File1, string File2)
{
  return rename(File1.c_str(), File2.c_str());
}

//
// String Coversions - temporary until CString is removed from codebase
//

string ConvertCStringToString(CString Str1)
{
  return (LPCTSTR)Str1;
}

CString ConvertStringToCString(string Str1)
{
  return Str1.c_str();
}

//
// Misc Functions
//

int GetTimeSeconds()
{
  const auto now     = std::chrono::system_clock::now();                        // Get the current time
  const auto epoch   = now.time_since_epoch();                                  // Transform the time into a duration since the epoch
  const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch); // Cast the duration into seconds
  int Seconds = (int)seconds.count();                                           // Get the number of seconds
  return Seconds;
}

//
// String Functions
//

int StrFindOneOf(CString HayStack, string Needle)
{
  string HayStack1 = ConvertCStringToString(HayStack);
  return HayStack1.find_first_of(Needle);
}

int StrFindOneOf(string HayStack, string Needle)
{
  return HayStack.find_first_of(Needle);
}

string StrGetWord(CString Str1, int WordNbr)
{
  return StrGetWord(ConvertCStringToString(Str1), WordNbr);
}

string StrGetWord(string Str1, int WordNbr)
{
  int    i;
  string Word;
  stringstream iss(Str1);
  i = 0;
  while (iss >> Word)
  {
    i++;
    if (i == WordNbr)
      return Word;
  }
  return "";
}

string StrGetWords(string Str1, int WordNbr)
{
  int    i;
  string Word;
  string Words;
  stringstream iss(Str1);
  Str1 += "\r\n";
  i = 1;
  while (iss >> Word)
  {
    i++;
    if (i == WordNbr)
    {
      getline(iss, Words);
      Words = StrTrimLeft(Words);
      return Words;
    }
  }
  return "";
}

string StrLeft(string Str1, int Len)
{
  return Str1.substr(0, Len);
}

string StrMakeLower(string Str1)
{
  transform(Str1.begin(), Str1.end(), Str1.begin(),
    [](unsigned char c) { return tolower(c); });
  return Str1;
}

void StrReplace(string& str, const string& from, const string& to)
{
  if (from.empty())
    return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

string StrRight(string Str1, int Len)
{
  if (Str1 == "")
  {
    return "";
  }
  return Str1.substr(Str1.length() - Len, Len);
}

string StrTrimLeft(string Str1)
{
  if (Str1 == "")
  {
    return "";
  }
  const auto First = Str1.find_first_not_of(" \r\n");
  if (First == string::npos) return "";
  return Str1.substr(First, Str1.length());
}

string StrTrimRight(string Str1)
{
  const auto Last = Str1.find_last_not_of(" \r\n");
  if (Last == string::npos) return "";
  return Str1.substr(0, Last + 1);
}

//
// Simplify calls to these functions in the rest of the codebase. e.g. SockRecv() vs Communication::SockRecv()
//

void AddMobToRoom(CString RoomId, CString MobileId)
{
  Mobile::AddMobToRoom(RoomId, MobileId);
}

bool AddObjToPlayerEqu(CString WearPosition, CString ObjectId)
{
  return Object::AddObjToPlayerEqu(WearPosition, ObjectId);
}

void AddObjToPlayerInv(Dnode* pDnodeTgt1, CString ObjectId)
{
  Object::AddObjToPlayerInv(pDnodeTgt1, ObjectId);
}

void AddObjToRoom(CString RoomId, CString ObjectId)
{
  Object::AddObjToRoom(RoomId, ObjectId);
}

void AdvanceTime()
{
  pCalendar->AdvanceTime();
}

void AppendIt()
{
  Descriptor::AppendIt();
}

int CalcAdjustedExpPoints(int PlayerLevel, int MobileLevel, int ExpPoints)
{
  return Player::CalcAdjustedExpPoints(PlayerLevel, MobileLevel, ExpPoints);
}

int CalcDamageToMobile(int Damage, int WeaponSkill)
{
  return Violence::CalcDamageToMobile(Damage, WeaponSkill);
}

int CalcDamageToPlayer(int Damage, int PAC)
{
  return Violence::CalcDamageToPlayer(Damage, PAC);
}

CString CalcHealthPct(int HitPoints, int HitPointsMax)
{
  return Violence::CalcHealthPct(HitPoints, HitPointsMax);
}

float CalcLevelExperience(int Level)
{
  return Player::CalcLevelExperience(Level);
}

int CalcPct(int Dividend, int Divisor)
{
  return Utility::CalcPct(Dividend, Divisor);
}

int CalcPlayerArmorClass()
{
  return Object::CalcPlayerArmorClass();
}

void ClearDescriptor()
{
  Descriptor::ClearDescriptor();
}

void CloseLogFile()
{
  Log::CloseLogFile();
}

int CountMob(CString MobileId)
{
  return Mobile::CountMob(MobileId);
}

void CreateMobPlayer(CString PlayerName, CString MobileId)
{
  Mobile::CreateMobPlayer(PlayerName, MobileId);
}

void CreateMobStatsFileWrite(CString Directory, CString MobileIdForFight, CString Stuff)
{
  Mobile::CreateMobStatsFileWrite(Directory, MobileIdForFight, Stuff);
}

void CreatePlayerMob(CString PlayerName, CString MobileId)
{
  Mobile::CreatePlayerMob(PlayerName, MobileId);
}

void DeleteMobPlayer(CString PlayerName, CString MobileId)
{
  Mobile::DeleteMobPlayer(PlayerName, MobileId);
}

void DeleteMobStats(CString MobileId)
{
  Mobile::DeleteMobStats(MobileId);
}

bool DeleteNode()
{
  return Descriptor::DeleteNode();
}

void DeletePlayerMob(CString PlayerName)
{
  Mobile::DeletePlayerMob(PlayerName);
}

CString DeleteWord(CString String, int WordNbr)
{
  return Utility::DeleteWord(String, WordNbr);
}

bool EndOfDnodeList()
{
  return Descriptor::EndOfDnodeList();
}

void Events()
{
  World::Events();
}

CString FormatCommas(CString String)
{
  return Utility::FormatCommas(String);
}

int GetCount()
{
  return Dnode::GetCount();
}

Dnode *GetDnode()
{
  return Descriptor::GetDnode();
}

string GetHomeDir()
{
  return Utility::GetHomeDir();
}

CString GetMobDesc1(CString MobileId)
{
  return Mobile::GetMobDesc1(MobileId);
}

int GetMobileArmor(CString MobileId)
{
  return Violence::GetMobileArmor(MobileId);
}

CString GetMobileAttack(CString MobileId)
{
  return Violence::GetMobileAttack(MobileId);
}

int GetMobileDamage(CString MobileId)
{
  return Violence::GetMobileDamage(MobileId);
}

CString GetMobileDesc1(CString MobileId)
{
  return Violence::GetMobileDesc1(MobileId);
}

CString GetMobileExpPointsLevel(CString MobileId)
{
  return Violence::GetMobileExpPointsLevel(MobileId);
}

CString GetMobileLoot(CString MobileId)
{
  return Violence::GetMobileLoot(MobileId);
}

CString GetMobileRoom(CString MobileId)
{
  return Violence::GetMobileRoom(MobileId);
}

CString GetMobPlayerMobileId(CString PlayerName, int i)
{
  return Violence::GetMobPlayerMobileId(PlayerName, i);
}

CString GetPlayerMobMobileId(CString PlayerName)
{
  return Violence::GetPlayerMobMobileId(PlayerName);
}

int GetRandomNumber(int Limit)
{
  return Utility::GetRandomNumber(Limit);
}

string GetRoomId(string RoomId)
{
  return Room::GetRoomId(RoomId);
}

string GetRoomName(string RoomId)
{
  return Room::GetRoomName(RoomId);
}

CString GetSqlStmt(CString SqlStmtId)
{
  return Utility::GetSqlStmt(SqlStmtId);
}

Dnode *GetTargetDnode(CString TargetName)
{
  return Communication::GetTargetDnode(TargetName);
}

string GetValidMobRoomExits(string RoomId)
{
  return Room::GetValidMobRoomExits(RoomId);
}

CString GetWord(CString String, int WordNbr)
{
  return Utility::GetWord(String, WordNbr);
}

CString GetWords(CString String, int StartWordNbr)
{
  return Utility::GetWords(String, StartWordNbr);
}

void HealMobiles()
{
  World::HealMobiles();
}

void InitDescriptor()
{
  Descriptor::InitDescriptor();
}

bool IsExit(string MudCmdIsExit)
{
  return Room::IsExit(MudCmdIsExit);
}

bool IsHelp()
{
  return Help::IsHelp();
}

Mobile *IsMobInRoom(CString MobileName)
{
  return Mobile::IsMobInRoom(MobileName);
}

Mobile *IsMobValid(CString MobileId)
{
  return Mobile::IsMobValid(MobileId);
}

bool IsMobileIdInRoom(CString RoomId, CString MobileId)
{
  return Mobile::IsMobileIdInRoom(RoomId, MobileId);
}

bool IsNameValid(CString Name)
{
  return Player::IsNameValid(Name);
}

bool IsNotWord(CString Word, CString WordList)
{
  return Utility::IsNotWord(Word, WordList);
}

void IsObjInPlayerEqu(CString ObjectName)
{
  Object::IsObjInPlayerEqu(ObjectName);
}

void IsObjInPlayerInv(CString ObjectName)
{
  Object::IsObjInPlayerInv(ObjectName);
}

void IsObjInRoom(CString ObjectName)
{
  Object::IsObjInRoom(ObjectName);
}

void IsObject(CString ObjectId)
{
  Object::IsObject(ObjectId);
}

bool IsPlayer(CString PlayerName)
{
  return Player::IsPlayer(PlayerName);
}

bool IsRoom(string RoomId)
{
  return Room::IsRoom(RoomId);
}

bool IsRoomType(string RoomId, string RoomType)
{
  return Room::IsRoomType(RoomId, RoomType);
}

bool IsShop(CString RoomId)
{
  return Shop::IsShop(RoomId);
}

void IsShopObj(CString RoomId, CString ObjectName)
{
  Shop::IsShopObj(RoomId, ObjectName);
}

bool IsSleeping()
{
  return Communication::IsSleeping();
}

bool IsSocial()
{
  return Social::IsSocial();
}

bool IsWord(CString Word, CString WordList)
{
  return Utility::IsWord(Word, WordList);
}

void ListObjects()
{
  Shop::ListObjects();
}

void LogIt(string LogBuf)
{
  Log::LogIt(LogBuf);
}

void LogIt(CString LogBuf)
{
  Log::LogIt(ConvertCStringToString(LogBuf));
}

CString MakeFirstLower(CString String)
{
  return Utility::MakeFirstLower(String);
}

CString MakeFirstUpper(CString String)
{
  return Utility::MakeFirstUpper(String);
}

void OpenLogFile()
{
  Log::OpenLogFile();
}

void Osi(CString ScriptType, CString ScriptId)
{
  World::Osi(ScriptType, ScriptId);
}

CString PronounSubstitute(CString MsgText)
{
  return Utility::PronounSubstitute(MsgText);
}

void PutMobBackInRoom(CString PlayerName, CString RoomId)
{
  Mobile::PutMobBackInRoom(PlayerName, RoomId);
}

void RemoveMobFromRoom(CString RoomId, CString MobileId)
{
  Mobile::RemoveMobFromRoom(RoomId, MobileId);
}

void RemoveObjFromPlayerEqu(CString ObjectId)
{
  Object::RemoveObjFromPlayerEqu(ObjectId);
}

void RemoveObjFromPlayerInv(CString ObjectId, int Count)
{
  Object::RemoveObjFromPlayerInv(ObjectId, Count);
}

void RemoveObjFromRoom(CString ObjectId)
{
  Object::RemoveObjFromRoom(ObjectId);
}

void SendToRoom(CString TargetRoomId, CString MsgText)
{
  Communication::SendToRoom(TargetRoomId, MsgText);
}

void SetpDnodeCursorFirst()
{
  Descriptor::SetpDnodeCursorFirst();
}

void SetpDnodeCursorNext()
{
  Descriptor::SetpDnodeCursorNext();
}

void ShowMobsInRoom(Dnode *pDnode)
{
  Mobile::ShowMobsInRoom(pDnode);
}

void ShowObjsInRoom(Dnode *pDnode)
{
  Object::ShowObjsInRoom(pDnode);
}

void ShowPlayerEqu(Dnode *pDnodeTgt1)
{
  Object::ShowPlayerEqu(pDnodeTgt1);
}

void ShowPlayerInv()
{
  Object::ShowPlayerInv();
}

void ShowPlayersInRoom(Dnode *pDnode)
{
  Communication::ShowPlayersInRoom(pDnode);
}

void ShowRoom(Dnode *pDnode)
{
  Room::ShowRoom(pDnode);
}

void SockCheckForNewConnections()
{
  Communication::SockCheckForNewConnections();
}

void SockClosePort(int PortNbr)
{
  Communication::SockClosePort(PortNbr);
}

void SockOpenPort(int PortNbr)
{
  Communication::SockOpenPort(PortNbr);
}

void SockRecv()
{
  Communication::SockRecv();
}

void SpawnMobileNoMove(CString MobileId)
{
  World::SpawnMobileNoMove(MobileId);
}

CString TranslateWord(CString Word)
{
  return Utility::TranslateWord(Word);
}

void UpdateMobInWorld(CString MobileId, CString AddRemove)
{
  Mobile::UpdateMobInWorld(MobileId, AddRemove);
}

bool ValidateIt(CString ValidationType)
{
  return Validate::ValidateIt(ValidationType);
}

CString WhackMobile(CString PlayerName, int DamageToMobile, CString MobileDesc1, CString WeaponType)
{
  return Violence::WhackMobile(PlayerName, DamageToMobile, MobileDesc1, WeaponType);
}

CString WhackPlayer(CString MobileDesc1, CString MobileAttack, int DamageToPlayer)
{
  return Violence::WhackPlayer(MobileDesc1, MobileAttack, DamageToPlayer);
}

void WhereMob(CString MobileIdSearch)
{
  Mobile::WhereMob(MobileIdSearch);
}

void WhereObj(CString ObjectIdSearch)
{
  Object::WhereObj(ObjectIdSearch);
}

int WordCount(CString String)
{
  return Utility::WordCount(String);
}