/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Log.h                                            *
* Usage:  Define Log class                                 *
* Author: Steve Bryant                                     *
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
    void    static  LogIt(string LogBuf);
    void    static  OpenLogFile();
};

#endif