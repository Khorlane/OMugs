/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Buffer.h                                         *
* Usage:  Define Buffer class                              *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef BUFFER_H
#define BUFFER_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Log.h"

/***********************************************************
* Define Buffer class                                      *
************************************************************/

class Buffer
{

// Public functions
  public:
    Buffer();
    ~Buffer();
    char            GetCurrentChar();
    char            GetNextChar();

// Private functions
  private:
    void            CloseScriptFile();
    void            GetScriptLine();
    void            OpenScriptFile();

// Private variables
  private:
    char            ch;
    int             InputPosition;
    ifstream        ScriptFile;
    int             Success;
};

#endif