/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Log.h                                            *
* Usage:  Define Log class                                 *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef LOG_H
#define LOG_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"

/***********************************************************
* Define Log class                                         *
************************************************************/

class Log  
{

// Public functions
  public:
    Log();
    ~Log();
    void    static  CloseLogFile();
    void    static  LogIt(CString LogBuf);
    void    static  LogIt(std::string LogBuf);
    void    static  OpenLogFile();
};

#endif