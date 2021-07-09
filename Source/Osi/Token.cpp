/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Token.cpp                                        *
* Usage:  Extracts tokens                                  *
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
#include "Token.h"

/***********************************************************
* Global variables                                         *
************************************************************/

/***********************************************************
* Token constructor                                        *
************************************************************/

Token::Token()
{
  ResWordCount           = 0;
  TokenCode              = tcDummy;
  TokenDataType          = tyDummy;
  TokenDataValue.Integer = 0;
  TokenString            = "";
  // Load reserved word table
  LoadResWordTable("float",  tcDeclareFloat);
  LoadResWordTable("int",    tcDeclareInt);
  LoadResWordTable("string", tcDeclareString);
  LoadResWordTable("else",   tcElse);
  LoadResWordTable("if",     tcIf);
  LoadResWordTable("Script", tcScript);
  LoadResWordTable("while",  tcWhile);
}

/***********************************************************
* Token destructor                                         *
************************************************************/

Token::~Token()
{
}

/***********************************************************
* Get end of file token                                    *
************************************************************/

void Token::GetEofToken(Buffer *pBuffer, Token *pToken)
{
  TokenCode = tcEndOfFile;
}

/***********************************************************
* Get error token                                          *
************************************************************/

void Token::GetErrorToken(Buffer *pBuffer, Token *pToken)
{
  TokenCode   = tcError;
  ch          = pBuffer->GetCurrentChar();
  TokenString = ch;
  pBuffer->GetNextChar();
  printf("\r\nUnrecognized token\r\n\r\n");
  exit(15);
}

/***********************************************************
* Get number token                                         *
************************************************************/

void Token::GetNumberToken(Buffer *pBuffer, Token *pToken)
{
  TokenCode              = tcError;
  ch                     = pBuffer->GetCurrentChar();
  DigitCount             = 0;
  DigitCountError        = false;
  TokenDataValue.Integer = 0;
  TokenString            = "";

  while (CharCodeMap[ch] == ccDigit)
  {
    TokenString += ch;
    if (++DigitCount <= MAX_DIGITS)
    {
      TokenDataValue.Integer = 10*TokenDataValue.Integer + (ch - '0');
    }
    else
    {
      DigitCountError = true;
    }
    ch = pBuffer->GetNextChar();
  }
  if (!DigitCountError)
  {
    TokenCode     = tcNumber;
    TokenDataType = tyInteger;
  }
}

/***********************************************************
* Get special token                                        *
************************************************************/

void Token::GetSpecialToken(Buffer *pBuffer, Token *pToken)
{
  TokenCode = tcError;
  ch = pBuffer->GetCurrentChar();
  TokenString = ch;
  switch (ch)
  {
    case '{' :
      TokenCode = tcBraceL;
      ch = pBuffer->GetNextChar();
      break;
    case '}' :
      TokenCode = tcBraceR;
      ch = pBuffer->GetNextChar();
      break;
    case '+' :
      TokenCode = tcPlus;
      ch = pBuffer->GetNextChar();
      break;
    case '-' :
      TokenCode = tcMinus;
      ch = pBuffer->GetNextChar();
      break;
    case '*' :
      TokenCode = tcStar;
      ch = pBuffer->GetNextChar();
      break;
    case '/' :
      TokenCode = tcSlash;
      ch = pBuffer->GetNextChar();
      break;
    case '(' :
      TokenCode = tcParenL;
      ch = pBuffer->GetNextChar();
      break;
    case ')' :
      TokenCode = tcParenR;
      ch = pBuffer->GetNextChar();
      break;
    case '=' :
      TokenCode = tcEqual;
      ch = pBuffer->GetNextChar();
      if (ch == '=')
      {
        TokenString += ch;
        TokenCode = tcEqualEqual;
        ch = pBuffer->GetNextChar();
      }
      break;
    case '!' :
      ch = pBuffer->GetNextChar();
      if (ch == '=')
      {
        TokenString += ch;
        TokenCode = tcNe;
        ch = pBuffer->GetNextChar();
      }
      break;
    case '<' :
      TokenCode = tcLt;
      ch = pBuffer->GetNextChar();
      if (ch == '=')
      {
        TokenString += ch;
        TokenCode = tcLe;
        ch = pBuffer->GetNextChar();
      }
      else
      if (ch == '>')
      {
        TokenString += ch;
        TokenCode = tcNe;
        ch = pBuffer->GetNextChar();
      }
      break;
    case '>' :
      TokenCode = tcGt;
      ch = pBuffer->GetNextChar();
      if (ch == '=')
      {
        TokenString += ch;
        TokenCode = tcGe;
        ch = pBuffer->GetNextChar();
      }
      break;
    default:
      TokenCode = tcError;
      ch = pBuffer->GetNextChar();
      LogBuf  = "Osi - Token::GetSpecialToken - Unrecognized special character - ";
      LogBuf += ScriptFileName.c_str();
      Log::LogIt(LogBuf);
      _endthread();
  }
}

/***********************************************************
* Get string token                                         *
************************************************************/

void Token::GetStringToken(Buffer *pBuffer, Token *pToken)
{
  TokenCode = tcError;
  ch = pBuffer->GetNextChar();
  TokenString = "";
  while (CharCodeMap[ch] != ccQuote)
  {
    TokenString += ch;
    ch = pBuffer->GetNextChar();
  }
  ch = pBuffer->GetNextChar();
  TokenCode = tcString;
    TokenDataType = tyString;
}

/***********************************************************
* Get token code                                           *
************************************************************/

EnumTokenCode Token::GetTokenCode()
{
  return TokenCode;
}

/***********************************************************
* Get token data type                                      *
************************************************************/

EnumDataType Token::GetTokenDataType()
{
  return TokenDataType;
}

/***********************************************************
* Get token value                                          *
************************************************************/

EnumDataValue Token::GetTokenDataValue()
{
  return TokenDataValue;
}

/***********************************************************
* Get token string                                         *
************************************************************/

CString Token::GetTokenString()
{
  return TokenString;
}

/***********************************************************
* Get word token                                           *
************************************************************/

void Token::GetWordToken(Buffer *pBuffer, Token *pToken)
{
  TokenCode = tcError;
  ch = pBuffer->GetCurrentChar();
  TokenString = "";
  while ((CharCodeMap[ch] == ccLetter) || (CharCodeMap[ch] == ccDigit))
  { // ch is a letter or digit
    TokenString += ch;
    ch = pBuffer->GetNextChar();
  }
  TokenCode = tcIdentifier;
  // Check for reserved word
  for (i = 0; i < ResWordCount; i++)
  { // loop thru reserved word table
    if (TokenString == ResWordTable[i].ResWord)
    { // Found reserved word
      TokenCode = ResWordTable[i].TokenCode;
    }
  }
}

/***********************************************************
* Load reserved word table                                 *
************************************************************/

void Token::LoadResWordTable(CString ResWord, EnumTokenCode TokenCode)
{
  if (ResWordCount >= MAX_RES_WORDS)
  {
    LogBuf  = "Osi - Token::LoadResWordTable - Reserved word table limit exceeded - ";
    LogBuf += ScriptFileName.c_str();
    Log::LogIt(LogBuf);
    _endthread();
  }
  ResWordTable[ResWordCount].ResWord   = ResWord;
  ResWordTable[ResWordCount].TokenCode = TokenCode;
  ResWordCount++;
}