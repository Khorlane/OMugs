/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Utility.h                                        *
* Usage:  General purpose utility stuff                    *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef UTILITY_H
#define UTILITY_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Dnode.h"
#include "Player.h"

/***********************************************************
* Define Utility class                                     *
************************************************************/

class Utility
{

// Public functions static
  public:
    static  int     CalcPct(int Dividend, int Divisor);
    static  CString FormatCommas(CString String);
    static  string  GetHomeDir();
    static  int     GetRandomNumber(int Limit);
    static  string  GetSqlStmt(string SqlStmtId);
    static  string  PronounSubstitute(string MsgText);
    static  string  TranslateWord(string Word);
};

#endif