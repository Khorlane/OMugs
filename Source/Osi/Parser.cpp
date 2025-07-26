/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Parser.cpp                                       *
* Usage:  Analyze and translate script per syntax rules    *
* Author: Steve Bryant                                     *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Parser.h"

/***********************************************************
* Globals                                                  *
************************************************************/

/***********************************************************
* Parser constructor                                       *
************************************************************/

Parser::Parser()
{
  pScanner    = new Scanner();
  pToken      = new Token;
  pSymbolRoot = new Symbol("");
  pIcode      = new Icode;
}

/***********************************************************
* Parser destructor                                        *
************************************************************/

Parser::~Parser()
{
  delete pScanner;
  delete pToken;
}

/***********************************************************
* Get token                                                *
************************************************************/

void Parser::GetToken()
{
  pScanner->GetToken(pToken);
  TokenCode = pToken->GetTokenCode();
}

/***********************************************************
* Get token and append it to the icode                     *
************************************************************/

void Parser::GetTokenAppend()
{
  GetToken();
  pIcode->PutTokenCode(TokenCode);
}

/***********************************************************
* Get icode pointer                                       *
************************************************************/

Icode *Parser::GetIcodePtr()
{
  return pIcode;
}

/***********************************************************
* Get symbol root pointer                                  *
************************************************************/

Symbol *Parser::GetSymbolRootPtr()
{
  return pSymbolRoot;
}

/***********************************************************
* Parse - recursive descent                                *
************************************************************/
void Parser::Parse()
{
  //pIcode->InsertLineMarker(0);
  GetTokenAppend();
  ParseScriptStatement();
  GetTokenAppend();
  //pIcode->InsertLineMarker(1);
  if (TokenCode != tcBraceL)
  { // '{' must follow 'Script' statement
    LogBuf  = "Osi - Parser::Parse - '{' must follow 'Script' statement -";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
 // ParseStatementList();
  ParseStatement();
  if (TokenCode != tcEndOfFile)
  { // Unexpected statements after last '}'
    LogBuf  = "Osi - Parser::Parse - '{' expected but not found - ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
  pIcode->InsertLineMarker(0);
}

/***********************************************************
* Parse script statement                                   *
************************************************************/
void Parser::ParseScriptStatement()
{
  pIcode->InsertLineMarker(0);
  if (TokenCode != tcScript)
  { // 'Script' must be first
    LogBuf  = "Osi - Parser::ParseScriptStatement - 'Script' must be first - ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
  GetTokenAppend();
  if (TokenCode != tcIdentifier)
  { // Script name must follow 'Script'
    LogBuf  = "Osi - Parser::ParseScriptStatement - Script name must follow 'Script' - ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
  // Put script name in symbol table
  pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
  if (!pSymbolCursor)
  { // Symbol is not found, enter it
    pSymbolInsert = Symbol::GetSymbolInsertPtr();
    pSymbolCursor = pSymbolInsert->EnterSymbol(pToken->TokenString);
  }
  pIcode->PutSymbolIndex(pSymbolCursor);
}

/***********************************************************
* Parse statement list                                     *
************************************************************/
void Parser::ParseStatementList()
{
  GetTokenAppend();
  //pIcode->InsertLineMarker(0);
  if (TokenCode == tcEndOfFile)
  {
    LogBuf  = "Osi - Parser::ParseStatementList - Unexpected end of file - ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
  while (TokenCode != tcBraceR)
  { // Parse until '}' is found
    ParseStatement();
    if (TokenCode == tcEndOfFile)
    {
      LogBuf  = "Osi - Parser::ParseStatementList - Unexpected end of file - ";
      LogBuf += ScriptFileName;
      LogIt(LogBuf);
      _endthread();
    }
  }
  pIcode->InsertLineMarker(-1);
  GetTokenAppend();
}

/***********************************************************
* Parse statement                                          *
************************************************************/
void Parser::ParseStatement()
{
  if (TokenCode == tcBraceL)
  { // Special case for one character token
    pIcode->InsertLineMarker(-1);
  }
  else
  { // Multi-character tokens
    pIcode->InsertLineMarker(0);
  }
  switch (TokenCode)
  {
    case tcBraceL :
    { // Beginning of a list of statements
      ParseStatementList();
      break;
    }
    case tcDeclareFloat :
    { // Declare a float variable
      ParseDeclareFloat();
      break;
    }
    case tcDeclareInt :
    { // Declare a integer variable
      ParseDeclareInt();
      break;
    }
    case tcDeclareString :
    { // Declare a string variable
      ParseDeclareString();
      break;
    }
    case tcIdentifier :
    { // It's an identifier, so it must be an assignment statement
      ParseAssignment();
      break;
    }
    case tcIf :
    { // while statement
      ParseIfStatement();
      break;
    }
    case tcWhile :
    { // while statement
      ParseWhileStatement();
      break;
    }
  }
}

/***********************************************************
* Parse declare int                                        *
************************************************************/
void Parser::ParseDeclareInt()
{
  pIcode->DecrementIcodeCursorPtr(); // Declares don't go in Icode
  GetToken();
  pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
  if (!pSymbolCursor)
  { // Symbol is not found, enter it
    pSymbolInsert = Symbol::GetSymbolInsertPtr();
    pSymbolCursor = pSymbolInsert->EnterSymbol(pToken->TokenString);
    pSymbolCursor->SymbolDataType = tyInteger;
    pSymbolCursor->SymbolType     = syVarInteger;
  }
  GetTokenAppend();
}

/***********************************************************
* Parse declare float                                      *
************************************************************/
void Parser::ParseDeclareFloat()
{
  pIcode->DecrementIcodeCursorPtr(); // Declares don't go in Icode
  GetToken();
  pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
  if (!pSymbolCursor)
  { // Symbol is not found, enter it
    pSymbolInsert = Symbol::GetSymbolInsertPtr();
    pSymbolCursor = pSymbolInsert->EnterSymbol(pToken->TokenString);
    pSymbolCursor->SymbolDataType = tyFloat;
    pSymbolCursor->SymbolType     = syVarFloat;
  }
  GetTokenAppend();
}

/***********************************************************
* Parse declare string                                     *
************************************************************/
void Parser::ParseDeclareString()
{
  pIcode->DecrementIcodeCursorPtr(); // Declares don't go in Icode
  GetToken();
  pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
  if (!pSymbolCursor)
  { // Symbol is not found, enter it
    pSymbolInsert = Symbol::GetSymbolInsertPtr();
    pSymbolCursor = pSymbolInsert->EnterSymbol(pToken->TokenString);
    pSymbolCursor->SymbolDataType = tyString;
    pSymbolCursor->SymbolType     = syVarString;
  }
  GetTokenAppend();
}

/***********************************************************
* Parse if statement                                       *
************************************************************/
void Parser::ParseIfStatement()
{
  char *pLocationMarkerIf;
  char *pLocationMarkerElse;

  pLocationMarkerIf = pIcode->PutLocationMarker();
  GetTokenAppend();
  ParseExpression();
  ParseStatement();
  pIcode->FixLocationMarker(pLocationMarkerIf);
  if (TokenCode == tcElse)
  { // Parse else
    pLocationMarkerElse = pIcode->PutLocationMarker();
    GetTokenAppend();
    ParseStatement();
    pIcode->FixLocationMarker(pLocationMarkerElse);
  }
}

/***********************************************************
* Parse while statement                                    *
************************************************************/
void Parser::ParseWhileStatement()
{
  char *pLocationMarker;

  pLocationMarker = pIcode->PutLocationMarker();
  GetTokenAppend();
  ParseExpression();
  ParseStatement();
  pIcode->FixLocationMarker(pLocationMarker);
}

/***********************************************************
* Parse assignment                                         *
************************************************************/
void Parser::ParseAssignment()
{
  pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
  if (!pSymbolCursor)
  { // Symbol not found, Variables must declared before being used
    LogBuf  = "Osi - Parser::ParseAssignment - Variables must declared before being used - ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
  pIcode->PutSymbolIndex(pSymbolCursor);
  GetTokenAppend();
  if (TokenCode != tcEqual)
  { // Equal sign must be next token
    LogBuf  = "Osi - Parser::ParseAssignment - Expecting equal sign - ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
    _endthread();
  }
  GetTokenAppend();
  // Found equal sign, parse the expression
  ParseExpression();
}

/***********************************************************
* Parse expression                                         *
************************************************************/
void Parser::ParseExpression()
{
  ParseSimpleExpression();
  if (   (TokenCode == tcEqualEqual)
      || (TokenCode == tcNe)
      || (TokenCode == tcLt)
      || (TokenCode == tcGt)
      || (TokenCode == tcLe)
      || (TokenCode == tcGe)
     )
  {
    GetTokenAppend();
    ParseSimpleExpression();
  }
}

/***********************************************************
* Parse simple expression                                  *
************************************************************/
void Parser::ParseSimpleExpression()
{
  if ((TokenCode == tcMinus) || (TokenCode ==tcPlus))
  { // Leading + or -
    GetTokenAppend();
  }
  ParseTerm();
  while ((TokenCode == tcMinus) || (TokenCode == tcPlus))
  {
    GetTokenAppend();
    ParseTerm();
  }
}

/***********************************************************
* Parse term                                               *
************************************************************/
void Parser::ParseTerm()
{
  ParseFactor();
  while ((TokenCode == tcStar) || (TokenCode == tcSlash))
  {
    GetTokenAppend();
    ParseFactor();
  }
}

/***********************************************************
* Parse factor                                             *
************************************************************/
void Parser::ParseFactor()
{
  switch (TokenCode)
  {
    case tcIdentifier :
    {
      pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
      if (pSymbolCursor)
      { // Symbol is found, add it to the icode
        pIcode->PutSymbolIndex(pSymbolCursor);
      }
      else
      { // Symbol not found, must be declared before it is used
        LogBuf  = "Osi - Parser::ParseFactor - Identifier is undefined - ";
        LogBuf += ScriptFileName;
        LogIt(LogBuf);
        _endthread();
      }
      GetTokenAppend();
      break;
    }
    case tcNumber :
    {
      pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
      if (!pSymbolCursor)
      { // Symbol is not found, enter it
        pSymbolInsert = Symbol::GetSymbolInsertPtr();
        pSymbolCursor = pSymbolInsert->EnterSymbol(pToken->TokenString);
      }
      pSymbolCursor->SymbolDataType = pToken->GetTokenDataType();
      if (pToken->TokenDataType == tyInteger)
      { // It's an integer, assign to symbol's integer value
        pSymbolCursor->SymbolDataValue.Integer = pToken->GetTokenDataValue().Integer;
        pSymbolCursor->SymbolType = syLitInteger;
      }
      else
      { // It's a float, assign to symbol's real value
        pSymbolCursor->SymbolDataValue.Real = pToken->GetTokenDataValue().Real;
        pSymbolCursor->SymbolType = syLitFloat;
      }
      pIcode->PutSymbolIndex(pSymbolCursor);
      GetTokenAppend();
      break;
    }
    case tcString :
    {
      pSymbolCursor = pSymbolRoot->SearchSymbol(pToken->TokenString);
      if (!pSymbolCursor)
      { // Symbol is not found, enter it
        pSymbolInsert = Symbol::GetSymbolInsertPtr();
        pSymbolCursor = pSymbolInsert->EnterSymbol(pToken->TokenString);
      }
      pSymbolCursor->SymbolDataType = pToken->GetTokenDataType();
      pSymbolCursor->SymbolType = syLitString;
      pIcode->PutSymbolIndex(pSymbolCursor);
      GetTokenAppend();
      break;
    }
    case tcParenL :
    { // Open paren
      GetTokenAppend();
      ParseExpression();
      if (TokenCode == tcParenR)
      { // Close paren
        GetTokenAppend();
      }
      else
      { // No close paren found
        LogBuf  = "Osi - Parser::ParseFactor - Missing right paren - ";
        LogBuf += ScriptFileName;
        LogIt(LogBuf);
        _endthread();
      }
      break;
    }
    default :
    { // Can't figure this one out, giving up
      LogBuf  = "Osi - Parser::ParseFactor - Invalid expression - ";
      LogBuf += ScriptFileName;
      LogIt(LogBuf);
      _endthread();
    }
  } // End switch
}