/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Log.cpp                                          *
* Usage:  Log messages to disk file                        *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Log.h"

/***********************************************************
* Globals                                                  *
************************************************************/


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

/***********************************************************
* Open log file                                            *
************************************************************/

void Log::OpenFile()
{
  int Success;
  char* LogFileName = ".\\Log\\HolyQuest.Log";

	Success = LogFile.Open( LogFileName,
							CFile::modeCreate     |   
							CFile::modeWrite      |
              CFile::shareDenyWrite |
							CFile::typeText);
	if(!Success)
  {
		exit(-1);
  }
}

/***********************************************************
* Write log file                                           *
************************************************************/

void Log::LogIt(CString LogStuff)
{
  LogStuff = LogStuff + "\n";
	LogFile.WriteString(LogStuff);
  LogFile.Flush();
}

/***********************************************************
* Close log file                                           *
************************************************************/

void Log::CloseFile()
{
  LogFile.Close();
}