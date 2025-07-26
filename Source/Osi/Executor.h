/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Executor.h                                       *
* Usage:  Define Executor class                            *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef EXECUTOR_H
#define EXECUTOR_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Icode.h"
#include "RunStack.h"
#include "Symbol.h"
#include "Token.h"

/***********************************************************
* Define Executor class                                    *
************************************************************/

class Executor
{

// Public functions
  public:
    Executor(Symbol *pSymbolRootIn, Icode *IcodeIn);
    ~Executor();
    void            Execute();

// Private functions
  private:
    void            GetToken();
    // Execute statements
    void            ExecuteStatement();
    void            ExecuteStatementList();
    void            ExecuteIfStatement();
    void            ExecuteWhileStatement();
    void            ExecuteAssignment();
    // Execute expressions
    void            ExecuteExpression();
    void            ExecuteSimpleExpression();
    void            ExecuteTerm();
    void            ExecuteFactor();

// Public variables
  public:
    int StatementCount;

// Private variables
  private:
    Icode          *pIcode;
    RunStack       *pRunStack;
    Symbol         *pSymbolCursor;
    Symbol         *pSymbolRoot;
    Token          *pToken;
    EnumTokenCode   TokenCode;
};

#endif