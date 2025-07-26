/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Log.cpp                                          *
* Usage:  Logs messages to disk file                       *
* Author: Steve Bryant                                     *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Log.h"

/***********************************************************
* Globals                                                  *
************************************************************/

ofstream LogFile;

/***********************************************************
* Log constructor                                          *
************************************************************/

Log::Log()
{
}

/***********************************************************
* Log destructor                                           *
************************************************************/

Log::~Log()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Close log file                                           *
************************************************************/

void Log::CloseLogFile()
{
  LogFile.close();
}

/***********************************************************
* Write log file                                           *
************************************************************/

void Log::LogIt(string LogBuf)
{
  string DisplayCurrentTime;

  time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  string s(30, '\0');
  strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S ", localtime(&now));
  DisplayCurrentTime = s;
  LogBuf  = DisplayCurrentTime + LogBuf;
  LogBuf += "\n";
  LogFile << LogBuf;
  LogFile.flush();
}

/***********************************************************
* Open log file                                            *
************************************************************/

void Log::OpenLogFile()
{
  string     LogFileName;
  string     LogSaveFileName;
  string     LogTime;
 
  PrintIt("Log::OpenLogFile()");
  LogFileName  = LOG_DIR;
  LogFileName += "SrvrLog.txt";
  if (FileExist(LogFileName))
  {
    sprintf(Buf, "%d", GetTimeSeconds());
    LogTime = Buf;

    LogSaveFileName  = StrLeft(LogFileName, StrGetLength(LogFileName)-4);
    LogSaveFileName += ".";
    LogSaveFileName += LogTime;
    LogSaveFileName += ".txt.";
    Rename(LogFileName, LogSaveFileName);
  }
  LogFile.open(LogFileName);
  if (!LogFile.is_open())
  {
    PrintIt("Log::OpenLogFile() - Failed");
    PrintIt("Hard Exit!");
    exit(1);
    _endthread();
  }
  PrintIt("Log File is Open");
}