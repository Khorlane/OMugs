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
    static  CString GetSqlStmt(CString SqlStmtId);
    static  CString GetWord(CString String, int WordNbr);
    static  CString GetWordPosLen(CString String, int WordNbr);
    static  CString GetWords(CString String, int WordNbr);
    static  bool    IsNotWord(CString Word, CString WordList);
    static  bool    IsWord(CString Word, CString WordList);
    static  string  PronounSubstitute(string MsgText);
    static  CString TranslateWord(CString Word);
    static  int     WordCount(CString String);
};

#endif