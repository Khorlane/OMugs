/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Executor.cpp                                     *
* Usage:  Execute statements                               *
* Author: Steve Bryant                                     *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Executor.h"

/***********************************************************
* Executor constructor                                     *
************************************************************/

Executor::Executor(Symbol *pSymbolRootIn, Icode *pIcodeIn)
{
  StatementCount = 0;
  pIcode         = pIcodeIn;
  pRunStack      = new RunStack;
  pSymbolCursor  = NULL;
  pSymbolRoot    = pSymbolRootIn;
  pToken         = new Token;
}

/***********************************************************
* Executor destructor                                      *
************************************************************/

Executor::~Executor()
{
  delete pIcode;
  delete pSymbolRoot;
}

/***********************************************************
* Get token from intermediate code                         *
************************************************************/

void Executor::GetToken()
{
  pIcode->GetToken(pToken);
  TokenCode     = pToken->TokenCode;
  pSymbolCursor = pIcode->GetSymbolCursorPtr();
}

/***********************************************************
* Execute the script                                       *
************************************************************/

void Executor::Execute()
{
  pIcode->ResetIcodeCursor();
  GetToken(); // tcScript     - 'Script'
  GetToken(); // tcIdentifier - Script Name
  GetToken(); // tcBraceL     - '{'
  while (TokenCode != tcEndOfFile)
  {
    ExecuteStatementList();
    if (TokenCode != tcEndOfFile)
    {
      GetToken();
    }
  }
}

/***********************************************************
* Execute statement list                                   *
************************************************************/
void Executor::ExecuteStatementList()
{
  GetToken();
  while (TokenCode != tcBraceR)
  { // Execute until '}' is found
    ExecuteStatement();
  }
  GetToken();
}

/***********************************************************
* Execute a statement                                      *
************************************************************/

void Executor::ExecuteStatement()
{
  switch (TokenCode)
  {
    case tcBraceL :
    { // Beginning of a list of statements
      ExecuteStatementList();
      break;
    }
    case tcIdentifier :
    {
      ExecuteAssignment();      // Like: Result = 1 + 2 * 3 / 2 + 4
      break;
    }
    case tcIf :
    {
      ExecuteIfStatement();     // Like: if (Count > 5)
      break;
    }
    case tcWhile :
    {
      ExecuteWhileStatement();  // Like: while (DoneFlag = 0)
      break;
    }
  }
}

/***********************************************************
* Execute an if statement                                  *
************************************************************/

void Executor::ExecuteIfStatement()
{
  char         *pIcodeCursorFalse;
  char         *pIcodeCursorNow;
  char         *pIcodeCursorSkipElse;
  unsigned char IcodeOffset;
  float         Result;

  GetToken();
  IcodeOffset       = pIcode->GetLocationMarker();
  pIcodeCursorNow   = pIcode->GetIcodeCursorPtr();
  pIcodeCursorFalse = pIcodeCursorNow + IcodeOffset;
  GetToken();
  ExecuteExpression();
  Result = pRunStack->Pop();
  if (Result)
  { // If statement's true path
    GetToken();
    ExecuteStatement();
    if (TokenCode == tcElse)
    { // Skip the else
      GetToken();
      IcodeOffset          = pIcode->GetLocationMarker();
      pIcodeCursorNow      = pIcode->GetIcodeCursorPtr();
      pIcodeCursorSkipElse = pIcodeCursorNow + IcodeOffset;
      pIcode->Goto(pIcodeCursorSkipElse);
    }
  }
  else
  { // If statement's false path
    pIcode->Goto(pIcodeCursorFalse);
    GetToken();
    if (TokenCode == tcElse)
    {
      GetToken();
      pIcode->GetLocationMarker();
      GetToken();
      ExecuteStatement();
    }
  }
}

/***********************************************************
* Execute a while statement                                *
************************************************************/

void Executor::ExecuteWhileStatement()
{
  char         *pIcodeCursorExpr;
  char         *pIcodeCursorNext;
  char         *pIcodeCursorNow;
  unsigned char IcodeOffset;
  float         Result;
  bool          WhileNotDone;

  GetToken();
  pIcodeCursorExpr = pIcode->GetIcodeCursorPtr();
  IcodeOffset      = pIcode->GetLocationMarker();
  pIcodeCursorNow  = pIcode->GetIcodeCursorPtr();
  pIcodeCursorNext = pIcodeCursorNow + IcodeOffset;
  WhileNotDone = true;
  while (WhileNotDone)
  {
    GetToken();
    ExecuteExpression();
    Result = pRunStack->Pop();
    if (Result)
    {
      GetToken();
      ExecuteStatement();
      pIcode->Goto(pIcodeCursorExpr);
    }
    else
    {
      WhileNotDone = false;
      pIcode->Goto(pIcodeCursorNext);
    }
    GetToken();
  }
}

/***********************************************************
* Execute an assignment                                    *
************************************************************/

void Executor::ExecuteAssignment()
{ // |||||||||||||||||||||||||||||||||
  // Result = 5 + 4 * 3 / (2 + 1) == 9 (Result is assigned 1)
  Symbol       *pSymbolToBeAssigned;
  Symbol       *pSymbolCursorString;
  unsigned char SymbolIndex;
  string        SymbolName;
  
  pSymbolToBeAssigned = pSymbolCursor;
  GetToken(); // '='
  GetToken(); // First token of expression
  ExecuteExpression();
  switch (pSymbolToBeAssigned->SymbolType)
  {
    case syVarInteger :
    {
      pSymbolToBeAssigned->SymbolDataValue.Integer = (int) pRunStack->Pop();
      break;
    }
    case syVarFloat :
    {
      pSymbolToBeAssigned->SymbolDataValue.Real = pRunStack->Pop();
      break;
    }
    case syVarString :
    {
      SymbolIndex = (int) pRunStack->Pop();
      pSymbolCursorString = pIcode->GetVectorSymbolCursorPtr(SymbolIndex);
      SymbolName = pSymbolCursorString->GetSymbolName();
      pSymbolToBeAssigned->SymbolDataValue.pString = &*SymbolName.begin();
      break;
    }
  }
}

/***********************************************************
* Execute an expression                                    *
************************************************************/

void Executor::ExecuteExpression()
{ //          ||||||||||||||||||||||||
  // Result = 5 + 4 * 3 / (2 + 1) == 9
  float         Operand1;
  float         Operand2;
  float         PushValue;
  EnumTokenCode RelationalOperator;

  ExecuteSimpleExpression();
  if (   (TokenCode == tcEqualEqual)
      || (TokenCode == tcNe)
      || (TokenCode == tcLt)
      || (TokenCode == tcGt)
      || (TokenCode == tcLe)
      || (TokenCode == tcGe)
     )
  { // It's a relational expression
    RelationalOperator = TokenCode;
    GetToken();
    ExecuteSimpleExpression();
    Operand2 = pRunStack->Pop();
    Operand1 = pRunStack->Pop();
    switch (RelationalOperator)
    {
      case tcEqualEqual :
      {
        if (Operand1 == Operand2)
        {
          PushValue = 1.0;
        }
        else
        {
          PushValue = 0.0;
        }
        pRunStack->Push(PushValue);
        break;
      }
      case tcNe :
      {
        if (Operand1 != Operand2)
        {
          PushValue = 1.0;
        }
        else
        {
          PushValue = 0.0;
        }
        pRunStack->Push(PushValue);
        break;
      }
      case tcLt :
      {
        if (Operand1 < Operand2)
        {
          PushValue = 1.0;
        }
        else
        {
          PushValue = 0.0;
        }
        pRunStack->Push(PushValue);
        break;
      }
      case tcGt :
      {
        if (Operand1 > Operand2)
        {
          PushValue = 1.0;
        }
        else
        {
          PushValue = 0.0;
        }
        pRunStack->Push(PushValue);
        break;
      }
      case tcLe :
      {
        if (Operand1 <= Operand2)
        {
          PushValue = 1.0;
        }
        else
        {
          PushValue = 0.0;
        }
        pRunStack->Push(PushValue);
        break;
      }
      case tcGe :
      {
        if (Operand1 >= Operand2)
        {
          PushValue = 1.0;
        }
        else
        {
          PushValue = 0.0;
        }
        pRunStack->Push(PushValue);
        break;
      }
    }
  }
}

/***********************************************************
* Execute a simple expression                              *
************************************************************/

void Executor::ExecuteSimpleExpression()
{ //            |            |        
  // Result = 5 + 4 * 3 / (2 + 1) == 9
  float         Operand1;
  float         Operand2;
  EnumTokenCode PlusMinusOperator;
  float         PushValue;
  EnumTokenCode UnaryOperator;

  UnaryOperator = tcPlus;
  if (   (TokenCode == tcPlus)
      || (TokenCode == tcMinus)
     )
  { // Unary operator found
    UnaryOperator = TokenCode;
    GetToken();
  }
  ExecuteTerm();
  if (UnaryOperator == tcMinus)
  { // Negate
    pRunStack->Push(-pRunStack->Pop());
  }
  while (   (TokenCode == tcPlus)
         || (TokenCode == tcMinus)
        )
  { // Do the + and -, put result on stack
    PlusMinusOperator = TokenCode;
    GetToken();
    ExecuteTerm();
    Operand2 = pRunStack->Pop();
    Operand1 = pRunStack->Pop();
    switch (PlusMinusOperator)
    {
      case tcPlus :
      {
        PushValue = Operand1 + Operand2;
        pRunStack->Push(PushValue);
        break;
      }
      case tcMinus :
      {
        PushValue = Operand1 - Operand2;
        pRunStack->Push(PushValue);
        break;
      }
    }
  }
}

/***********************************************************
* Execute a term                                           *
************************************************************/

void Executor::ExecuteTerm()
{ //                |   |             
  // Result = 5 + 4 * 3 / (2 + 1) == 9
  EnumTokenCode MultiplyDivideOperator;
  float         Operand1;
  float         Operand2;
  float         PushValue;

  ExecuteFactor();
  while (   (TokenCode == tcStar)
         || (TokenCode == tcSlash)
        )
  { // Do the * and /, put result on stack
    MultiplyDivideOperator = TokenCode;
    GetToken();
    ExecuteFactor();
    Operand2 = pRunStack->Pop();
    Operand1 = pRunStack->Pop();
    switch (MultiplyDivideOperator)
    {
      case tcStar :
      {
        PushValue = Operand1 * Operand2;
        pRunStack->Push(PushValue);
        break;
      }
      case tcSlash :
      {
        if (Operand2 != 0.0)
        {
          PushValue = Operand1 / Operand2;
          pRunStack->Push(PushValue);
          break;
        }
        else
        {
          LogBuf  = "Osi - Executor::ExecuteTerm - Divide by zero runtime error ";
          LogIt(LogBuf);
        }
      }
    }
  }
}

/***********************************************************
* Execute a factor                                         *
************************************************************/

void Executor::ExecuteFactor()
{ //          |   |   |   ||   ||    |
  // Result = 5 + 4 * 3 / (2 + 1) == 9
  float PushValue;

  switch (TokenCode)
  {
    case tcIdentifier :
    case tcNumber :
    {
      PushValue = (float) pSymbolCursor->SymbolDataValue.Integer;
      pRunStack->Push(PushValue);
      GetToken();
      break;
    }
    case tcString :
    {
      PushValue = (float) pSymbolCursor->GetSymbolIndex();
      pRunStack->Push(PushValue);
      GetToken();
      break;
    }
    case tcParenL :
    {
      GetToken(); // First token of expression
      ExecuteExpression();
      GetToken(); // Close paren
      break;
    }
  }
}