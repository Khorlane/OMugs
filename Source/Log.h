/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Log.h                                            *
* Usage:  Define Log class                                 *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef LOG_H
#define LOG_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"

/***********************************************************
* Define Log class                                         *
************************************************************/

class Log  
{

// Public functions
  public:
	  Log();
    ~Log();
	  void          CloseFile();
	  void          LogIt(CString LogStuff);
	  void          OpenFile();

// Private variables
  private:
    CStdioFile    LogFile;
};

#endif