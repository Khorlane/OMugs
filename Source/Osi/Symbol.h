/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Symbol.h                                         *
* Usage:  Define Symbol class                              *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef SYMBOL_H
#define SYMBOL_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"

/***********************************************************
* Define Symbol class                                      *
************************************************************/

class Symbol
{

// Public functions
  public:
    Symbol(string Name);
    ~Symbol();

    Symbol         *EnterSymbol(string Name);
    static  int     GetSymbolCount();
    int             GetSymbolIndex();
    static  Symbol *GetSymbolInsertPtr();
    string          GetSymbolName();
    Symbol         *SearchSymbol(string Name);
    void            ConvertSymbolTable(Symbol *vpNodes[]);

// Public variables
  public:
    //float           SymbolValue;
    EnumDataType    SymbolDataType;
    EnumDataValue   SymbolDataValue;
    EnumSymbolType  SymbolType;

// Private variables static
  private:
    static  Symbol *pSymbolCursor;
    static  Symbol *pSymbolInsert;
    static  int     SymbolCount;

// Private variables
  private:
    Symbol         *pSymbolLeft;
    Symbol         *pSymbolNew;
    Symbol         *pSymbolRight;
    string          SymbolName;
    unsigned char   SymbolIndex;
};

#endif