/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Scanner.h                                        *
* Usage:  Define Scanner class                             *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef SCANNER_H
#define SCANNER_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Buffer.h"
#include "Config.h"
#include "Token.h"

/***********************************************************
* Define Scanner class                                     *
************************************************************/

class Scanner
{

// Public functions
  public:
    Scanner();
    ~Scanner();
    void            GetToken(Token *pToken);

// Private functions
  private:
    void            SkipWhiteSpace();

// Private variables
  private:
    Buffer         *pBuffer;
    char            ch;
    int             i;
};

#endif