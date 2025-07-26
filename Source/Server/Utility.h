/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Utility.h                                        *
* Usage:  General purpose utility stuff                    *
* Author: Steve Bryant                                     *
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
    static  string  FormatCommas(string String);
    static  string  GetHomeDir();
    static  int     GetRandomNumber(int Limit);
    static  string  GetSqlStmt(string SqlStmtId);
    static  string  PronounSubstitute(string MsgText);
    static  string  TranslateWord(string Word);
};

#endif