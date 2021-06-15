/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Parser.h                                         *
* Usage:  Define Parser class                              *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef PARSER_H
#define PARSER_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Icode.h"
#include "Scanner.h"
#include "Symbol.h"

using namespace std;

/***********************************************************
* Define Parser class                                      *
************************************************************/

class Parser
{

// Public functions
  public:
    Parser();
    ~Parser();
    Icode          *GetIcodePtr();
    Symbol         *GetSymbolRootPtr();
    void            Parse();

// Private functions
  private:
    void            GetToken();
    void            GetTokenAppend();
    // Parse statements
    void            ParseStatement();
    void            ParseScriptStatement();
    void            ParseStatementList();
    void            ParseDeclareInt();
    void            ParseDeclareFloat();
    void            ParseDeclareString();
    void            ParseIfStatement();
    void            ParseWhileStatement();
    void            ParseAssignment();
    // Pase expressions
    void            ParseExpression();
    void            ParseSimpleExpression();
    void            ParseTerm();
    void            ParseFactor();

// Private variables
  private:
    Icode          *pIcode;
    Scanner        *pScanner;
    Symbol         *pSymbolCursor;
    Symbol         *pSymbolInsert;
    Symbol         *pSymbolRoot;
    Token          *pToken;
    CString         LogBuf;
    EnumTokenCode   TokenCode;
};

#endif