/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Buffer.cpp                                       *
* Usage:  Script I/O routines                              *
* Author: Steve Bryant                                     *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Buffer.h"

/***********************************************************
* Global variables                                         *
************************************************************/

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
  ScriptFile.close();
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
  if (InputPosition >= (int) StrGetLength(Stuff))
  { // End of line
    GetScriptLine();
  }
  if (Stuff != "EndScript")
  { // Get next character
    ch = Stuff[InputPosition];
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
  if (ScriptFile.peek() == EOF)
  { // Unexpected end of file
    LogBuf  = "Osi - Buffer::GetScriptLine - Unexpected end of file ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
  }
  getline(ScriptFile, Stuff);
  CurrentLineNumber++;
  while (ScriptFile.peek() != EOF)
  { // Skip blank lines
    if (Stuff != "")
    {
      break;
    }
    getline(ScriptFile, Stuff);
    CurrentLineNumber++;
  }
  InputPosition = 0;
}

/***********************************************************
* Open script file                                         *
************************************************************/

void Buffer::OpenScriptFile()
{
  ScriptFile.open(ScriptFileName);
  if (!ScriptFile.is_open())
  {
    LogBuf  = "Osi - Buffer::OpenScriptFile - Failed to open ";
    LogBuf += ScriptFileName;
    LogIt(LogBuf);
  }
}