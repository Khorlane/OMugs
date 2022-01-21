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

CStdioFile      LogFile;

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
  LogFile.Close();
}

/***********************************************************
* Write log file                                           *
************************************************************/

void Log::LogIt(CString LogBuf)
{
  CString DisplayCurrentTime;

  time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  string s(30, '\0');
  strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S ", localtime(&now));
  DisplayCurrentTime = ConvertStringToCString(s);
  LogBuf  = DisplayCurrentTime + LogBuf;
  LogBuf += "\n";
  LogFile.WriteString(LogBuf);
  LogFile.Flush();
}

void Log::LogIt(string LogBuf)
{
  CString csLogBuf;
  csLogBuf = ConvertStringToCString(LogBuf);
  LogIt(csLogBuf);
}

/***********************************************************
* Open log file                                            *
************************************************************/

void Log::OpenLogFile()
{
  CTime       CurrentTime;
  CFileStatus FileStatus;
  CString     LogFileName;
  CString     LogSaveFileName;
  CString     LogTime;
  int         Success;
 
  LogFileName  = LOG_DIR;
  LogFileName += "SrvrLog.txt";
  if (CFile::GetStatus(LogFileName, FileStatus))
  {
    const auto now = std::chrono::system_clock::now();                            // Get the current time
    const auto epoch = now.time_since_epoch();                                    // Transform the time into a duration since the epoch
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch); // Cast the duration into seconds
    sprintf(Buf, "%d", (int) seconds.count());                                    // Get the number of seconds
    LogTime = ConvertStringToCString(Buf);
    LogSaveFileName  = LogFileName.Left(LogFileName.GetLength()-4);
    LogSaveFileName += ".";
    LogSaveFileName += LogTime;
    LogSaveFileName += ".txt.";
    CFile::Rename(LogFileName, LogSaveFileName);
  }
  Success = LogFile.Open(LogFileName,
              CFile::modeCreate     |   
              CFile::modeWrite      |
              CFile::shareDenyWrite |
              CFile::typeText);
  if(!Success)
  {
    AfxMessageBox("Log::OpenLogFile - Failed", MB_ICONSTOP);
    _endthread();
  }
}