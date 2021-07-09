/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Scanner.cpp                                      *
* Usage:  Scans for tokens                                 *
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
#include "Config.h"
#include "Scanner.h"

/***********************************************************
* Global variables                                         *
************************************************************/

/***********************************************************
* Scanner constructor                                      *
************************************************************/

Scanner::Scanner()
{
  pBuffer = new Buffer();
  // Initialize character code map
  for (i =   0; i <  128; ++i)
    CharCodeMap[i] = ccError;
  for (i = 'a'; i <= 'z'; ++i)
    CharCodeMap[i] = ccLetter;
  for (i = 'A'; i <= 'Z'; ++i)
    CharCodeMap[i] = ccLetter;
  for (i = '0'; i <= '9'; ++i)
    CharCodeMap[i] = ccDigit;

  CharCodeMap['{' ]     = ccSpecial;
  CharCodeMap['}' ]     = ccSpecial;
  CharCodeMap['+' ]     = ccSpecial;
  CharCodeMap['-' ]     = ccSpecial;
  CharCodeMap['*' ]     = ccSpecial;
  CharCodeMap['/' ]     = ccSpecial;
  CharCodeMap['(' ]     = ccSpecial;
  CharCodeMap[')' ]     = ccSpecial;
  CharCodeMap['=' ]     = ccSpecial;
  CharCodeMap['<' ]     = ccSpecial;
  CharCodeMap['>' ]     = ccSpecial;
  CharCodeMap['!' ]     = ccSpecial;
/*CharCodeMap['^' ]     = ccSpecial;
  CharCodeMap['.' ]     = ccSpecial;
  CharCodeMap[',' ]     = ccSpecial;
  CharCodeMap['[' ]     = ccSpecial;
  CharCodeMap[']' ]     = ccSpecial;
  CharCodeMap[':' ]     = ccSpecial;
  CharCodeMap[';' ]     = ccSpecial;
*/
  CharCodeMap[' ' ]     = ccWhiteSpace;
  CharCodeMap['\t']     = ccWhiteSpace;
  CharCodeMap['\n']     = ccWhiteSpace;
  CharCodeMap['\0']     = ccWhiteSpace;

  CharCodeMap['\"']     = ccQuote;
  CharCodeMap[EOF_CHAR] = ccEndOfFile;
}

/***********************************************************
* Scanner destructor                                       *
************************************************************/

Scanner::~Scanner()
{
  delete pBuffer;
}

/***********************************************************
* Get token pointer                                        *
************************************************************/
void Scanner::GetToken(Token *pToken)
{
  SkipWhiteSpace();
  ch = pBuffer->GetCurrentChar();
  switch(CharCodeMap[ch])
  {
    case ccLetter:
      pToken->GetWordToken(pBuffer, pToken);
      break;
    case ccDigit:
      pToken->GetNumberToken(pBuffer, pToken);
      break;
    case ccQuote:
      pToken->GetStringToken(pBuffer, pToken);
      break;
    case ccSpecial:
      pToken->GetSpecialToken(pBuffer, pToken);
      break;
    case ccEndOfFile:
      pToken->GetEofToken(pBuffer, pToken);
      break;
    default:
      pToken->GetErrorToken(pBuffer, pToken);
      break;
  }
}

/***********************************************************
* Skip white space                                         *
************************************************************/
void Scanner::SkipWhiteSpace()
{
  ch = pBuffer->GetCurrentChar();
  while ((CharCodeMap[ch] == ccWhiteSpace) || (ch == '['))
  {
    if (CharCodeMap[ch] == ccWhiteSpace)
    {
      ch = pBuffer->GetNextChar();
    }
    else
    {
      if (ch == '[')
      {
        while ((ch != ']') && (ch != EOF_CHAR))
        {
          ch = pBuffer->GetNextChar();
        }
        if (ch != EOF_CHAR)
        {
          ch = pBuffer->GetNextChar();
        }
        else
        { // Unexpected end of file
          LogBuf  = "Osi - Scanner::SkipWhiteSpace - Unexpected end of file - ";
          LogBuf += ScriptFileName.c_str();
          Log::LogIt(LogBuf);
          _endthread();
        }
      }
    }
  }
}