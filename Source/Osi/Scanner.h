/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Scanner.h                                        *
* Usage:  Define Scanner class                             *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef SCANNER_H
#define SCANNER_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Buffer.h"
#include "Config.h"
#include "Token.h"

using namespace std;

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
    CString         LogBuf;

};

#endif