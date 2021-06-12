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

#include "stdafx.h"         // precompiled headers
#include "BigDog.h"

using namespace std;

/***********************************************************
* Globals                                                  *
************************************************************/

CString   HomeDir; 
string    ScriptFileName;
CString 	SqlStatement;

Calendar *pCalendar;
sqlite3  *pWorldDb;
bool      StateConnections;
bool      StateRunning;
bool      StateStopping;
float     PACMN; // Percent Armor Class Magic Number

string ConvertCStringToString(CString Str1)
{
  return (LPCTSTR)Str1;
}

CString ConvertStringToCString(string Str1)
{
  return Str1.c_str();
}

string StrLeft(string Str1, int Len)
{
  return Str1.substr(0, Len);
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
  const auto First = Str1.find_first_not_of(' ');
  return Str1.substr(First,Str1.length());
}

string StrTrimRight(string Str1)
{
  const auto Last = Str1.find_last_not_of(' ');
  return Str1.substr(0, Last+1);
}

string StrMakeLower(string Str1)
{
  transform(Str1.begin(), Str1.end(), Str1.begin(),
    [](unsigned char c) { return tolower(c); });
  return Str1;
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

/***********************************************************
 * BigDog                                                  *
 ***********************************************************/

void BigDog()
{
  WhoIsOnline *pWhoIsOnline;
  CString      ErrorMsg;
  int          EventTick;
  CString      GoGoGoFileName;
  CString      LogBuf;
  int          MobHealTick;
  CFileStatus  FileStatus;
  CString      Sqlite3DatabaseFileName;
  int          Sqlite3Result;
  CString      StopItFileName;
  CString      TmpStr;
  bool         ValErr;
  int          WhoIsOnlineTick;

  if (_chdir(HomeDir))
  { // Change directory failed
    AfxMessageBox("BigDog - Change directory to HomeDir failed", MB_ICONSTOP);
    _endthread();
  }
  // Set Go Stop, force go status
  StopItFileName  = CONTROL_DIR;
  StopItFileName += "StopIt";
  GoGoGoFileName  = CONTROL_DIR;
  GoGoGoFileName += "GoGoGo";
  if (CFile::GetStatus(StopItFileName, FileStatus))
  { // If StopIt file exists, Rename it to GoGoGo
    CFile::Rename(StopItFileName, GoGoGoFileName);
  }
  // Log game startup
  Log::OpenLogFile();
  LogBuf  = "OMugs version ";
  LogBuf += VERSION ;
  LogBuf += " has started";
  Log::LogIt(LogBuf);
  LogBuf  = "Home directory is ";
  LogBuf += HomeDir;
  Log::LogIt(LogBuf);
  // Initialize
  EventTick        = EVENT_TICK;    // Force first tick at startup
  MobHealTick      = 0;
  WhoIsOnlineTick  = 0;
  StateConnections = true;
  StateRunning     = true;
  StateStopping    = false;
  srand((unsigned)time(NULL)); // Seed random number generator
  PACMN = 1.0f / MAC * MDRP / 100.0f;
  // Open Sqlite3 database
  Sqlite3DatabaseFileName = SQLITE3_DATABASE_DIR;
  Sqlite3DatabaseFileName += "World.db";
  if (!CFile::GetStatus(Sqlite3DatabaseFileName, FileStatus))
  { // Sqlite3 database not found
    AfxMessageBox("BigDog - Sqlite3 database file not found", MB_ICONSTOP);
    Log::CloseLogFile();
    _endthread();
  }
  Sqlite3Result = sqlite3_open(Sqlite3DatabaseFileName, &pWorldDb);
  if(Sqlite3Result != SQLITE_OK)
  {
		TmpStr.Format("%s", sqlite3_errmsg(pWorldDb));
    ErrorMsg = "Can't open database: " + TmpStr;
    AfxMessageBox(ErrorMsg);
    Log::CloseLogFile();
    _endthread();
  }
  // Validate library files
  ValErr = Validate::ValidateIt("All");
  if (ValErr)
  { // Validation failed
    Log::LogIt("OMugs has stopped");
    Log::CloseLogFile();
    return;
  }
  // Validation was ok, so open port, init, play on
  Communication::SockOpenPort(PORT_NBR);
  Descriptor::InitDescriptor();
  pCalendar = new Calendar;
  while (StateRunning)
  { // Game runs until it is stopped
    Sleep(MILLI_SECONDS_TO_SLEEP);
    Utility::DoSqlStatement("Begin");
    pCalendar->AdvanceTime();
    if (!StateStopping)
    { // Game is not stopping, but should it be?
      if (CFile::GetStatus(StopItFileName, FileStatus))
      { // StopIt file was found, Stop the game
        StateStopping = true;
        LogBuf = "Game is stopping";
        Log::LogIt(LogBuf);
      }
    }
    if (!StateStopping)
    { // No new connections after stop command
      Communication::SockCheckForNewConnections();
      if (StateConnections && Dnode::GetCount() == 1)
      { // No players connected
        Log::LogIt("No Connections - going to sleep");
        StateConnections = false;
      }
    }
    if (StateConnections)
    { // One or more players are connected
      Communication::SockRecv();
      EventTick++;
      if (EventTick >= EVENT_TICK)
      { // Time to process events
        EventTick = 0;
        World::Events();
      }
      MobHealTick++;
      if (MobHealTick >= MOB_HEAL_TICK)
      { // Time to process events
        MobHealTick = 0;
        World::HealMobiles();
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
    Utility::DoSqlStatement("Commit");
  }
  // Game has stopped so clean up
  Descriptor::ClearDescriptor();
  Communication::SockClosePort(PORT_NBR);
  pWhoIsOnline = new WhoIsOnline(HomeDir);
  delete pWhoIsOnline;
  delete pCalendar;
  Sqlite3Result = sqlite3_close(pWorldDb);
  if (Sqlite3Result == 0)
  {
    Log::LogIt("Sqlite3 database closed");
  }
  else
  {
    AfxMessageBox("BigDog - Sqlite3 database close failed", MB_ICONSTOP);
    _endthread();
  }
  Log::LogIt("OMugs has stopped");
  Log::CloseLogFile();
}