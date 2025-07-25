/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Token.h                                          *
* Usage:  Define Token class                               *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef TOKEN_H
#define TOKEN_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Buffer.h"

/***********************************************************
* Define Token class                                       *
************************************************************/

class Token
{

// Friend classes
  friend class Icode;
  friend class Parser;
  friend class Executor;

// Public functions
  public:
    Token();
    ~Token();
    // Get tokens
    void            GetWordToken(Buffer *pBuffer, Token *pToken);
    void            GetNumberToken(Buffer *pBuffer, Token *pToken);
    void            GetStringToken(Buffer *pBuffer, Token *pToken);
    void            GetSpecialToken(Buffer *pBuffer, Token *pToken);
    void            GetEofToken(Buffer *pBuffer, Token *pToken);
    void            GetErrorToken(Buffer *pBuffer, Token *pToken);
    // Get token variables
    EnumTokenCode   GetTokenCode();
    EnumDataType    GetTokenDataType();
    EnumDataValue   GetTokenDataValue();
    string          GetTokenString();

// Private functions
  private:
    void            LoadResWordTable(string ResWord, EnumTokenCode TokenCode);

// Private variables
  private:
    char            ch;
    int             DigitCount;
    bool            DigitCountError;
    int             i;
    EnumTokenCode   TokenCode;      // tcBraceL, tcBraceR, tcElse, etc.....
    EnumDataType    TokenDataType;  // tyInteger, tyFloat, tyCharacter, tyString
    EnumDataValue   TokenDataValue; //   .Integer, .Real,   .Character, .pString
    string          TokenString;    // Variable names, literals, strings, operators (+ - * / etc)
    int             ResWordCount;
    ResWordStruct   ResWordTable[MAX_RES_WORDS];
};

#endif