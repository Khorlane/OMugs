/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Log.cpp                                          *
* Usage:  Logs messages to disk file                       *
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

void Log::LogIt(CString LogBuf)
{
  LogIt(ConvertCStringToString(LogBuf));
}

/***********************************************************
* Open log file                                            *
************************************************************/

void Log::OpenLogFile()
{
  string     LogFileName;
  string     LogSaveFileName;
  string     LogTime;
 
  LogFileName  = LOG_DIR;
  LogFileName += "SrvrLog.txt";
  if (FileExist(LogFileName))
  {
    sprintf(Buf, "%d", GetTimeSeconds());
    LogTime = Buf;

    LogSaveFileName  = StrLeft(LogFileName, LogFileName.length()-4);
    LogSaveFileName += ".";
    LogSaveFileName += LogTime;
    LogSaveFileName += ".txt.";
    Rename(LogFileName, LogSaveFileName);
  }
  LogFile.open(LogFileName);
  if(!LogFile.is_open())
  {
    AfxMessageBox("Log::OpenLogFile - Failed", MB_ICONSTOP);
    _endthread();
  }
}