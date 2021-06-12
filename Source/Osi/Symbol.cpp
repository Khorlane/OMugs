/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Symbol.cpp                                       *
* Usage:  Manages the symbol table                         *
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

#include "stdafx.h"         // precompiled headers
#include "Symbol.h"

/***********************************************************
* Global variables                                         *
************************************************************/

Symbol *Symbol::pSymbolCursor = NULL;
Symbol *Symbol::pSymbolInsert = NULL;
int     Symbol::SymbolCount   = 0;

/***********************************************************
* Symbol constructor                                       *
************************************************************/

Symbol::Symbol(CString Name)
{
  pSymbolLeft  = NULL;
  pSymbolRight = NULL;
  SymbolIndex  = SymbolCount;
  SymbolName   = Name;
}

/***********************************************************
* Symbol destructor                                        *
************************************************************/

Symbol::~Symbol()
{
  delete pSymbolLeft;
  delete pSymbolRight;
}

/***********************************************************
* Enter a symbol into the symbol table                     *
************************************************************/

Symbol *Symbol::EnterSymbol(CString Name)
{
  SymbolCount++;
  if (SymbolCount == 1)
  { // This is the root of the symbol table
    pSymbolCursor              = this;
    pSymbolCursor->SymbolName  = Name;
    pSymbolCursor->SymbolIndex = SymbolCount;
    return pSymbolCursor;
  }
  else
  if (Name < SymbolName)
  { // New symbol goes to the left of this symbol
    pSymbolCursor              = this;
    pSymbolNew                 = new Symbol(Name);
    pSymbolCursor->pSymbolLeft = pSymbolNew;
    pSymbolCursor              = pSymbolNew;
    pSymbolCursor->SymbolIndex = SymbolCount;
    return pSymbolCursor;
  }
  else
  if (Name > SymbolName)
  { // New symbol goes to the right of this symbol
    pSymbolCursor               = this;
    pSymbolNew                  = new Symbol(Name);
    pSymbolCursor->pSymbolRight = pSymbolNew;
    pSymbolCursor               = pSymbolNew;
    pSymbolCursor->SymbolIndex  = SymbolCount;
    return pSymbolCursor;
  }
  return NULL; // This should never happen
}

/***********************************************************
* Get symbol count                                         *
************************************************************/
int Symbol::GetSymbolCount()
{
  return SymbolCount;
}

/***********************************************************
* Get symbol table insert node pointer                     *
************************************************************/
Symbol *Symbol::GetSymbolInsertPtr()
{
  return pSymbolInsert;
}

/***********************************************************
* Get symbol's index value                                 *
************************************************************/
int Symbol::GetSymbolIndex()
{
  return SymbolIndex;
}

/***********************************************************
* Get symbol's name                                        *
************************************************************/
CString Symbol::GetSymbolName()
{
  return SymbolName;
}


/***********************************************************
* Search for symbol in the symbol table                    *
************************************************************/

Symbol *Symbol::SearchSymbol(CString Name)
{
  pSymbolCursor = this;
  while (pSymbolCursor)
  {
    if (Name == pSymbolCursor->SymbolName)
    { // Found symbol
      break;
    }
    else
    if (Name < pSymbolCursor->SymbolName)
    { // Continue search on left
      if (!pSymbolCursor->pSymbolLeft)
      { // Not found
        pSymbolInsert = pSymbolCursor;
      }
      pSymbolCursor = pSymbolCursor->pSymbolLeft;
    }
    else
    if (Name > pSymbolCursor->SymbolName)
    { // Continue search on right
      if (!pSymbolCursor->pSymbolRight)
      { // Not found
        pSymbolInsert = pSymbolCursor;
      }
      pSymbolCursor = pSymbolCursor->pSymbolRight;
    }
  }
  return pSymbolCursor; // Will be NULL, if not found
}

/***********************************************************
* Convert the symbol table                                 *
************************************************************/

void Symbol::ConvertSymbolTable(Symbol *vpSymbol[])
{
  if (pSymbolLeft)
  { // Convert left subtree
    pSymbolLeft->ConvertSymbolTable(vpSymbol);
  }
  // Set symbol pointer
  vpSymbol[SymbolIndex] = this;
  if (pSymbolRight)
  { // Convert right subtree
    pSymbolRight->ConvertSymbolTable(vpSymbol);
  }
}