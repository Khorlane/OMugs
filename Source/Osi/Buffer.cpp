/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Buffer.cpp                                       *
* Usage:  Script I/O routines                              *
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
#include "Buffer.h"

using namespace std;

/***********************************************************
* Global variables                                         *
************************************************************/

extern string       ScriptFileName;
unsigned char CurrentLineNumber;

/***********************************************************
* Buffer constructor                                       *
************************************************************/

Buffer::Buffer()
{
  CurrentLineNumber = 0;
  OpenScriptFile();
  GetScriptLine();
  GetNextChar();
}

/***********************************************************
* Buffer destructor                                        *
************************************************************/

Buffer::~Buffer()
{
  CloseScriptFile();
}

/***********************************************************
* Close script file                                        *
************************************************************/

void Buffer::CloseScriptFile()
{
  ScriptFile.Close();
}

/***********************************************************
* Get current char                                         *
************************************************************/

char Buffer::GetCurrentChar()
{
  return ch;
}

/***********************************************************
* Get next char                                            *
************************************************************/

char Buffer::GetNextChar()
{
  if (InputPosition >= Stuff.GetLength())
  { // End of line
    GetScriptLine();
  }
  if (Stuff != "EndScript")
  { // Get next character
    ch = Stuff.GetAt(InputPosition);
  }
  else
  { // End of script
    ch = EOF_CHAR;
  }
  InputPosition++;
  return ch;
}

/***********************************************************
* Get script line                                          *
************************************************************/

void Buffer::GetScriptLine()
{
  if (ScriptFile.GetPosition() == ScriptFile.GetLength())
  { // Unexpected end of file
    LogBuf  = "Osi - Buffer::GetScriptLine - Unexpected end of file ";
    LogBuf += ScriptFile.GetFileName();
    Log::LogIt(LogBuf);
  }
  ScriptFile.ReadString(Stuff);
  CurrentLineNumber++;
  while (Stuff == "")
  { // Skip blank lines
    ScriptFile.ReadString(Stuff);
    CurrentLineNumber++;
  }
  InputPosition = 0;
}

/***********************************************************
* Open script file                                         *
************************************************************/

void Buffer::OpenScriptFile()
{
  Success = ScriptFile.Open(ScriptFileName.c_str(),
                 CFile::modeRead |
                 CFile::typeText);
  if(!Success)
  {
    LogBuf  = "Osi - Buffer::OpenScriptFile - Failed to open ";
    LogBuf += ScriptFileName.c_str();
    Log::LogIt(LogBuf);
  }
}