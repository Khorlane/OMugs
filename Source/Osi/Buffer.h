/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Buffer.h                                         *
* Usage:  Define Buffer class                              *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
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
    string          LogBuf;
    ifstream        ScriptFile;
    string          Stuff;
    int             Success;
};

#endif