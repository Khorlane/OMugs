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
  string       StopItFileName;
  int          WhoIsOnlineTick;

  PrintIt("OMugs Starting");
  ErrorCode = ChgDir(HomeDir);
  if (ErrorCode.value() != 0)
  { // Change directory failed
    PrintIt("BigDog() - Change directory to HomeDir failed");
    exit(1);
  }
  // Set Go Stop, force Go status
  StopItFileName  = CONTROL_DIR;
  StopItFileName += "StopIt";
  GoGoGoFileName  = CONTROL_DIR;
  GoGoGoFileName += "GoGoGo";
  if (FileExist(StopItFileName))
  { // If StopIt file exists, Rename it to GoGoGo
    ErrorCode = Rename(StopItFileName, GoGoGoFileName);
    if (ErrorCode.value() != 0)
    {
      PrintIt("BigDog() - Rename of 'StopIt' to 'GoGoGo' failed!");
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
  // Put test code here. It will be executed first, even before dialogs are presented.
}

//
// File Functions
//

error_code ChgDir(string Dir)
{
  error_code ErrorCode;
  
  fs::current_path(Dir, ErrorCode);
  return ErrorCode;
}

bool FileExist(string Name)
{
  bool Exist;

  ifstream f(Name);
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

error_code Rename(string File1, string File2)
{
  error_code ErrorCode;

  fs::rename(File1, File2, ErrorCode);
  return ErrorCode;
}

error_code Remove(string File1)
{
  error_code ErrorCode;

  fs::remove(File1, ErrorCode);
  return ErrorCode;
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

void PrintIt(string Message)
{
  Message = "\r\n" + Message + "\r\n";
  cout << Message;
}

//
// String Functions
//

// Count characters in a string (temporary)
int StrCountChar(CString Str1, char c)
{
  string x;
  x = ConvertCStringToString(Str1);
  return StrCountChar(x, c);
}

// Count characters in a string
int StrCountChar(string Str1, char c)
{
  return count(Str1.begin(), Str1.end(), c);
}

int StrCountWords(CString Str1)
{
  string x;
  x = ConvertCStringToString(Str1);
  return StrCountWords(x);
}

// Count number of words in a string
int StrCountWords(string Str1)
{
  string S1, TmpStr;
  vector<string> Words;

  S1 = StrSqueeze(Str1);
  stringstream SS1(S1);
  while (getline(SS1, TmpStr, ' '))
  {
    Words.push_back(TmpStr);
  }
  return Words.size();
}

// Delete characters from a string (temporary)
CString StrDelete(CString Str1, int Position, int Length)
{
  return ConvertStringToCString(StrDelete(ConvertCStringToString(Str1), Position, Length));
}

// Delete characters from a string
string StrDelete(string Str1, int Position, int Length)
{
  return Str1.erase(Position, Length);
}

// Delete the word specified by WordNbr and Squeeze() (temporary)
CString StrDeleteWord(CString Str1, int WordNbr)
{
  return ConvertStringToCString(StrDeleteWord(ConvertCStringToString(Str1), WordNbr));
}

// Delete the word specified by WordNbr and Squeeze()
string StrDeleteWord(string Str1, int WordNbr)
{
  string S1, TmpStr;
  vector<string> Words;

  S1 = StrSqueeze(Str1);
  stringstream SS1(S1);
  while (getline(SS1, TmpStr, ' '))
  {
    Words.push_back(TmpStr);
  }
  Str1.clear();
  Words.erase(Words.begin() + WordNbr - 1);
  for (auto const& s : Words) { Str1 += s; Str1 += " "; }
  return Str1;
}

// Find first occurrence of Needle in HayStack (temporary)
int StrFind(CString HayStack, CString Needle)
{
  return StrFind(ConvertCStringToString(HayStack), ConvertCStringToString(Needle));
}

// Find first occurrence of Needle in HayStack (temporary)
int StrFind(CString HayStack, const char *Needle)
{
  return StrFind(ConvertCStringToString(HayStack), Needle);
}

// Find first occurrence of Needle in HayStack
int StrFind(string HayStack, string Needle)
{
  return HayStack.find(Needle);
}

// Find first occurrence of a character in a string (Temporary)
int StrFindFirstChar(CString Str1, char c)
{
  return StrFindFirstChar(ConvertCStringToString(Str1), c);
}

// Find first occurrence of a character in a string
int StrFindFirstChar(string Str1, char c)
{
  return Str1.find_first_of(c);
}

// Find one of the characters specified in Needle in the HayStack (temporary)
int StrFindOneOf(CString HayStack, string Needle)
{
  return StrFindOneOf(ConvertCStringToString(HayStack), Needle);
}

// Find one of the characters specified in Needle in the HayStack
int StrFindOneOf(string HayStack, string Needle)
{
  return HayStack.find_first_of(Needle);
}

// Return the character in Str1 at Position (temporary)
char StrGetAt(CString Str1, int Position)
{
  return StrGetAt(ConvertCStringToString(Str1), Position);
}

// Return the character in Str1 at Position
char StrGetAt(string Str1, int Position)
{
  return Str1[Position];
}

// Get length of a string (temporary)
int StrGetLength(CString Str1)
{
  return StrGetLength(ConvertCStringToString(Str1));
}

// Get length of a string
int StrGetLength(string Str1)
{
  return Str1.length();
}

// Get the Word indicated by WordNbr (temporary)
CString StrGetWord(CString Str1, int WordNbr)
{
  return ConvertStringToCString(StrGetWord(ConvertCStringToString(Str1), WordNbr));
}

// Get the Word indicated by WordNbr
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

// Get the rest of the Words in a string starting with the Word indicated by WordNbr
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

// Insert string Str2 into string Str1 at Position (temporary)
CString StrInsert(CString Str1, int Position, CString Str2)
{
  return ConvertStringToCString(StrInsert(ConvertCStringToString(Str1), Position, ConvertCStringToString(Str2)));
}

// Insert string Str2 into string Str1 at Position
string  StrInsert(string Str1, int Position, string Str2)
{
  return Str1.insert(Position, Str2);
}

// Insert character c into string Str1 at Position (temporary)
CString StrInsertChar(CString Str1, int Position, char c)
{
  return ConvertStringToCString(StrInsertChar(ConvertCStringToString(Str1), Position, c));
}

// Insert character c into string Str1 at Position
string StrInsertChar(string Str1, int Position, char c)
{
  return Str1.insert(Position, 1, c);
}

// Get the left portion of a string (temporary)
CString StrLeft(CString Str1, int Len)
{
  return ConvertStringToCString(StrLeft(ConvertCStringToString(Str1), Len));
}

// Get the left portion of a string
string StrLeft(string Str1, int Len)
{
  return Str1.substr(0, Len);
}

// Make First Letter Lowercase (temporary)
CString StrMakeFirstLower(CString Str1)
{
  return ConvertStringToCString(StrMakeFirstLower(ConvertCStringToString(Str1)));
}

// Make First Letter Lowercase
string StrMakeFirstLower(string Str1)
{
  Str1[0] = tolower(Str1[0]);
  return Str1;
}

// Make First Letter Uppercase (temporary)
CString StrMakeFirstUpper(CString Str1)
{
  return ConvertStringToCString(StrMakeFirstUpper(ConvertCStringToString(Str1)));
}

// Make First Letter Uppercase
string StrMakeFirstUpper(string Str1)
{
  Str1[0] = toupper(Str1[0]);
  return Str1;
}

// Lower case the whole string (temporary)
CString StrMakeLower(CString Str1)
{
  return ConvertStringToCString(StrMakeLower(ConvertCStringToString(Str1)));
}

// Lower case the whole string
string StrMakeLower(string Str1)
{
  transform(Str1.begin(), Str1.end(), Str1.begin(),
    [](unsigned char c) { return tolower(c); });
  return Str1;
}

// Upper case the whole string (temporary)
CString StrMakeUpper(CString Str1)
{
  string x;
  x = ConvertCStringToString(Str1);
  x = StrMakeUpper(x);
  return ConvertStringToCString(x);
}

// Upper case the whole string
string StrMakeUpper(string Str1)
{
  transform(Str1.begin(), Str1.end(), Str1.begin(),
    [](unsigned char c) { return toupper(c); });
  return Str1;
}

CString StrRemove(CString Str1, char c)
{
  string x;
  x = ConvertCStringToString(Str1);
  x = StrRemove(x, c);
  return ConvertStringToCString(x);
}

// Remove all occurrences of a character from a string
string StrRemove(string Str1, char c)
{
  Str1.erase(remove(Str1.begin(), Str1.end(), c), Str1.end());
  return Str1;
}

// Replace a string in a string (temporary)
void StrReplace(CString &str, const string &from, const string &to)
{
  string x;
  x = ConvertCStringToString(str);
  StrReplace(x, from, to);
  str = ConvertStringToCString(x);
}

// Replace a string in a string
void StrReplace(string &str, const string &from, const string &to)
{
  if (from.empty())
    return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

// Get the right portion of a string (temporary)
CString StrRight(CString Str1, int Len)
{
  return ConvertStringToCString(StrRight(ConvertCStringToString(Str1), Len));
}

// Get the right portion of a string
string StrRight(string Str1, int Len)
{
  if (Str1 == "")
  {
    return "";
  }
  return Str1.substr(Str1.length() - Len, Len);
}

// Replace the character in Str1 at Position (temporary)
CString StrSetAt(CString Str1, int Position, char c)
{
  return ConvertStringToCString(StrSetAt(ConvertCStringToString(Str1), Position, c));
}

// Replace the character in Str1 at Position
string StrSetAt(string Str1, int Position, char c)
{
  Str1[Position] = c;
  return Str1;
}

// Remove leading, trailing, and extra spaces
CString StrSqueeze(CString Str1)
{
  return ConvertStringToCString(StrSqueeze(ConvertCStringToString(Str1)));
}

// Remove leading, trailing, and extra spaces
string StrSqueeze(string Str1)
{
  size_t EndOfString = std::string::npos;

  Str1 = StrTrimLeft(Str1);
  Str1 = StrTrimRight(Str1);
  while (Str1.find("  ") != EndOfString)
  {
    StrReplace(Str1, "  ", " ");
  }
  return Str1;
}

// Remove leading whitespace (temporary)
CString StrTrimLeft(CString Str1)
{
  string x;
  x = ConvertCStringToString(Str1);
  x = StrTrimLeft(x);
  return ConvertStringToCString(x);
}

// Remove leading whitespace
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

// Remove trailing whitespace (temporary)
CString StrTrimRight(CString Str1)
{
  string x;
  x = ConvertCStringToString(Str1);
  x = StrTrimRight(x);
  return ConvertStringToCString(x);
}

// Remove trailing whitespace
string StrTrimRight(string Str1)
{
  const auto Last = Str1.find_last_not_of(" \r\n");
  if (Last == string::npos) return "";
  return Str1.substr(0, Last + 1);
}

// Get the string in StrVector1 at Position
string  StrVectorGetAt(vector<string> &StrVector1, int Position) // TODO - steve - This function might not be needed
{
  return StrVector1[Position];
}

// Replace the string in StrVector1 at Position
void StrVectorSetAt(vector<string> &StrVector1, int Position, string Str1) // TODO - steve - This function might not be needed
{
  int x = StrVector1.size();
  x--;
  if (Position > x)
  {
    StrVector1.push_back(Str1);
    return;
  }
  StrVector1[Position] = Str1;
}

//
// Simplify calls to these functions in the rest of the codebase. e.g. SockRecv() vs Communication::SockRecv()
//

void AddMobToRoom(string RoomId, string MobileId)
{
  Mobile::AddMobToRoom(RoomId, MobileId);
}

bool AddObjToPlayerEqu(string WearPosition, string ObjectId)
{
  return Object::AddObjToPlayerEqu(WearPosition, ObjectId);
}

void AddObjToPlayerInv(Dnode* pDnodeTgt1, string ObjectId)
{
  Object::AddObjToPlayerInv(pDnodeTgt1, ObjectId);
}

void AddObjToRoom(string RoomId, string ObjectId)
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

int CountMob(string MobileId)
{
  return Mobile::CountMob(MobileId);
}

void CreateMobPlayer(string PlayerName, string MobileId)
{
  Mobile::CreateMobPlayer(PlayerName, MobileId);
}

void CreateMobStatsFileWrite(string Directory, string MobileIdForFight, string Stuff)
{
  Mobile::CreateMobStatsFileWrite(Directory, MobileIdForFight, Stuff);
}

void CreatePlayerMob(string PlayerName, string MobileId)
{
  Mobile::CreatePlayerMob(PlayerName, MobileId);
}

void DeleteMobPlayer(string PlayerName, string MobileId)
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

int GetMobileArmor(string MobileId)
{
  return Violence::GetMobileArmor(MobileId);
}

string GetMobileAttack(string MobileId)
{
  return Violence::GetMobileAttack(MobileId);
}

int GetMobileDamage(string MobileId)
{
  return Violence::GetMobileDamage(MobileId);
}

string GetMobileDesc1(string MobileId)
{
  return Violence::GetMobileDesc1(MobileId);
}

string GetMobileExpPointsLevel(string MobileId)
{
  return Violence::GetMobileExpPointsLevel(MobileId);
}

string GetMobileLoot(string MobileId)
{
  return Violence::GetMobileLoot(MobileId);
}

string GetMobileRoom(string MobileId)
{
  return Violence::GetMobileRoom(MobileId);
}

string GetMobPlayerMobileId(string PlayerName, int i)
{
  return Violence::GetMobPlayerMobileId(PlayerName, i);
}

string GetPlayerMobMobileId(string PlayerName)
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

Dnode *GetTargetDnode(string TargetName)
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

Mobile *IsMobValid(string MobileId)
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

void IsObjInPlayerEqu(string ObjectName)
{
  Object::IsObjInPlayerEqu(ObjectName);
}

void IsObjInPlayerInv(string ObjectName)
{
  Object::IsObjInPlayerInv(ObjectName);
}

void IsObjInRoom(string ObjectName)
{
  Object::IsObjInRoom(ObjectName);
}

void IsObject(string ObjectId)
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

bool IsShop(string RoomId)
{
  return Shop::IsShop(RoomId);
}

void IsShopObj(string RoomId, string ObjectName)
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

void OpenLogFile()
{
  Log::OpenLogFile();
}

void Osi(string ScriptType, string ScriptId)
{
  World::Osi(ScriptType, ScriptId);
}

string PronounSubstitute(string MsgText)
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

void RemoveObjFromPlayerEqu(string ObjectId)
{
  Object::RemoveObjFromPlayerEqu(ObjectId);
}

void RemoveObjFromPlayerInv(string ObjectId, int Count)
{
  Object::RemoveObjFromPlayerInv(ObjectId, Count);
}

void RemoveObjFromRoom(string ObjectId)
{
  Object::RemoveObjFromRoom(ObjectId);
}

void SendToRoom(CString TargetRoomId, CString MsgText)
{
  Communication::SendToRoom(TargetRoomId, MsgText);
}

void SendToRoom(string TargetRoomId, string MsgText)
{
  Communication::SendToRoom(ConvertStringToCString(TargetRoomId), ConvertStringToCString(MsgText));
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

void SpawnMobileNoMove(string MobileId)
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

string WhackMobile(string PlayerName, int DamageToMobile, string MobileDesc1, string WeaponType)
{
  return Violence::WhackMobile(PlayerName, DamageToMobile, MobileDesc1, WeaponType);
}

string WhackPlayer(string MobileDesc1, string MobileAttack, int DamageToPlayer)
{
  return Violence::WhackPlayer(MobileDesc1, MobileAttack, DamageToPlayer);
}

void WhereMob(CString MobileIdSearch)
{
  Mobile::WhereMob(MobileIdSearch);
}

void WhereObj(string ObjectIdSearch)
{
  Object::WhereObj(ObjectIdSearch);
}

int WordCount(CString String)
{
  return Utility::WordCount(String);
}