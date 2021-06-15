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

#include "stdafx.h"         // precompiled headers
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
  CTime   CurrentTime;
  CString DisplayCurrentTime;

  CurrentTime = CTime::GetCurrentTime();
  DisplayCurrentTime = CurrentTime.Format("%Y/%m/%d %H:%M:%S ");
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
 
  LogFileName = LOG_DIR;
  LogFileName += "SrvrLog.txt";
  if (CFile::GetStatus(LogFileName, FileStatus))
  {
    CurrentTime = CTime::GetCurrentTime();
    LogTime.Format("%d", CurrentTime);
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