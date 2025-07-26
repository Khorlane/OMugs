/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Icode.h                                          *
* Usage:  Define Icode class                               *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef ICODE_H
#define ICODE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Symbol.h"
#include "Token.h"

/***********************************************************
* Define Icode class                                       *
************************************************************/

class Icode
{

// Friend classes
  friend class Executor;

// Public functions
  public:
    Icode();
    ~Icode();
    void            DecrementIcodeCursorPtr();
    void            FixLocationMarker(char *pIcodeOffset);
    char           *GetIcodeCursorPtr();
    unsigned char   GetLocationMarker();
    void            GetToken(Token *pToken);
    Symbol         *GetSymbolCursorPtr();
    Symbol         *GetVectorSymbolCursorPtr(unsigned char SymbolIndex);
    void            Goto(char *pIcodeCursorNew);
    void            InsertLineMarker(int LineNumberOffset);
    char           *PutLocationMarker();
    void            PutTokenCode(EnumTokenCode TokenCode);
    void            PutSymbolIndex(Symbol *pSymbolCursor);
    void            ResetIcodeCursor();
    void            SetSymbolVector(Symbol **vpSymbol);

// Private variables
  private:
    Symbol        **vpSymbol;
    char           *pIcodeCursor;
    char           *pIcodeFixOffset;
    Symbol         *pSymbolCursor;
    EnumTokenCode   mcLineMarker;
    EnumTokenCode   mcLocationMarker;
    char            IcodeSeg[ICODE_SEGSIZE];
    char            IcodeToken;
    unsigned char   IcodeOffset;
    char            LastCode;
    unsigned char   Offset;
    unsigned char   SymbolIndex;
    EnumTokenCode   TokenCode;
};

#endif