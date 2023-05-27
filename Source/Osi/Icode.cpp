/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Icode.cpp                                        *
* Usage:  Manages the intermediate code                    *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h" // This is only here to speed up compiles
#include "Icode.h"

/***********************************************************
* Global                                                   *
************************************************************/

// Vector for tokens not found in symbol table
const char *SymbolStrings[] =
{
  "",           // tcDummy,
  "{",          // tcBraceL,
  "}",          // tcBraceR,
  "int",        // tcDeclareInt,
  "float",      // tcDeclareFloat,
  "string",     // tcDeclareString,
  "else",       // tcElse,
  "",           // tcEndOfFile,
  "=",          // tcEqual,
  "==",         // tcEqualEqual,
  "",           // tcError,
  ">=",         // tcGe,
  ">",          // tcGt,
  "",           // tcIdentifier,
  "if",         // tcIf,
  "<=",         // tcLe,
  "<",          // tcLt,
  "-",          // tcMinus,
  "!=",         // tcNe,
  "",           // tcNumber,
  "(",          // tcParenL,
  ")",          // tcParenR,
  "+",          // tcPlus,
  "Script",     // tcScript,
  "/",          // tcSlash,
  "*",          // tcStar,
  "",           // tcString,
  "while",      // tcWhile
};

/***********************************************************
* Icode constructor                                        *
************************************************************/

Icode::Icode()
{
  mcLineMarker     = ((EnumTokenCode) 127);
  mcLocationMarker = ((EnumTokenCode) 126);
  pIcodeCursor     = &IcodeSeg[0];
}

/***********************************************************
* Icode destructor                                         *
************************************************************/

Icode::~Icode()
{
}

/***********************************************************
* Decrement icode cursor pointer                           *
************************************************************/
void Icode::DecrementIcodeCursorPtr()
{
  pIcodeCursor--;
}


/***********************************************************
* Get icode cursor pointer                                 *
************************************************************/

char *Icode::GetIcodeCursorPtr()
{
  return pIcodeCursor;
}

/***********************************************************
* Get a location marker from the intermediate code         *
************************************************************/

unsigned char Icode::GetLocationMarker()
{
  IcodeOffset = *pIcodeCursor;
  pIcodeCursor++;
  return IcodeOffset;
}

/***********************************************************
* Get a token from the intermediate code                   *
************************************************************/

void Icode::GetToken(Token *pToken)
{
  IcodeToken = *pIcodeCursor;
  TokenCode = (EnumTokenCode) IcodeToken;
  pIcodeCursor++;
  while(TokenCode == mcLineMarker)
  { // Process line markers to get current line number
    CurrentLineNumber = *pIcodeCursor;
    pIcodeCursor++;
    IcodeToken = *pIcodeCursor;
    TokenCode = (EnumTokenCode) IcodeToken;
    pIcodeCursor++;
  }
  if (TokenCode == mcLocationMarker)
  { // It's location marker
    pSymbolCursor = NULL;
    pToken->TokenString = "";
    return;
  }
  pToken->TokenCode = TokenCode;
  switch (TokenCode)
  {
    case tcIdentifier :
    case tcNumber :
    case tcString :
      pSymbolCursor = GetVectorSymbolCursorPtr(0);
      pToken->TokenString = pSymbolCursor->GetSymbolName();
      break;
    default :
      pSymbolCursor = NULL;
      pToken->TokenString = SymbolStrings[TokenCode];
  }
}

/***********************************************************
* Get the symbol cursor pointer                            *
************************************************************/

Symbol *Icode::GetSymbolCursorPtr()
{
  return pSymbolCursor;
}

/***********************************************************
* Get symbol pointer from the intermediate code via vector *
************************************************************/

Symbol *Icode::GetVectorSymbolCursorPtr(unsigned char SymbolIndex)
{
  if (SymbolIndex == 0)
  { // No index, get it from icode
    SymbolIndex   = *pIcodeCursor;
  }
  pSymbolCursor = vpSymbol[SymbolIndex];
  pIcodeCursor++;
  return pSymbolCursor;
}

/***********************************************************
* Reposition icode cursor                                  *
************************************************************/

void Icode::Goto(char *pIcodeCursorNew)
{
  pIcodeCursor = pIcodeCursorNew;
}

/***********************************************************
* Fix location marker offset in the intermediate           *
************************************************************/

void Icode::FixLocationMarker(char *pIcodeOffset)
{
  Offset = pIcodeCursor - pIcodeOffset - 2;
  *pIcodeFixOffset = Offset;
}

/***********************************************************
* Insert line marker into the intermediate code            *
************************************************************/

void Icode::InsertLineMarker(int LineNumberOffset)
{
  if (pIcodeCursor > &IcodeSeg[0])
  { // Save last code, insert line marker, then add line marker
    pIcodeCursor--;
    LastCode = *pIcodeCursor;
    *pIcodeCursor = mcLineMarker;
    pIcodeCursor++;
    *pIcodeCursor = CurrentLineNumber + LineNumberOffset;
    pIcodeCursor++;
    *pIcodeCursor = LastCode;
    pIcodeCursor++;
  }
  else
  { // First time thru, no last code to save, just add line marker
    *pIcodeCursor = mcLineMarker;
    pIcodeCursor++;
    *pIcodeCursor = CurrentLineNumber - LineNumberOffset;
    pIcodeCursor++;
  }
}

/***********************************************************
* Append a location marker to the intermediate code        *
************************************************************/

char *Icode::PutLocationMarker()
{
  *pIcodeCursor = mcLocationMarker;
  pIcodeCursor++;
  *pIcodeCursor = 0;
  pIcodeFixOffset = pIcodeCursor;
  pIcodeCursor++;
  return pIcodeFixOffset;
}

/***********************************************************
* Append a token code to the intermediate code             *
************************************************************/

void Icode::PutTokenCode(EnumTokenCode TokenCode)
{
  *pIcodeCursor = TokenCode;
  pIcodeCursor++;
}

/***********************************************************
* Append a symbol index to the intermediate code           *
************************************************************/

void Icode::PutSymbolIndex(Symbol *pSymbolCursor)
{
  SymbolIndex   = pSymbolCursor->GetSymbolIndex();
  *pIcodeCursor = SymbolIndex;
  pIcodeCursor++;
}

/***********************************************************
* Reset the intermediate code cursor                       *
************************************************************/

void Icode::ResetIcodeCursor()
{
  pIcodeCursor = &IcodeSeg[0];
}

/***********************************************************
* Set the symbol table vector                              *
************************************************************/

void Icode::SetSymbolVector(Symbol **vpSymbolIn)
{
  vpSymbol = vpSymbolIn;
}