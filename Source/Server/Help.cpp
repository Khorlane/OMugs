/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Help.cpp                                         *
* Usage:  Displays help file entries to player             *
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
#include "Help.h"

/***********************************************************
* Globals                                                  *
************************************************************/

ifstream   HelpFile;
string     HelpText;

/***********************************************************
* Help constructor                                         *
************************************************************/

Help::Help()
{
}

/***********************************************************
* Help destructor                                          *
************************************************************/

Help::~Help()
{
}

////////////////////////////////////////////////////////////
// Public functions static                                //
////////////////////////////////////////////////////////////

/***********************************************************
* Is it a Help topic?                                      *
************************************************************/

bool Help::IsHelp()
{
  bool    Found;
  string  HelpLookup;

  Found = false;
  if (!OpenFile())
  { // If the file isn't there, then all Helps are not found, doh!
    return false;
  }
  HelpLookup = StrGetWord(CmdStr, 2);
  HelpLookup = StrMakeLower(HelpLookup);
  HelpText = "Not Done";
  while (HelpText != "End of Help")
  { // Loop until Help is found or end of file
    ReadLine();
    HelpText = StrTrimLeft(HelpText);
    TmpStr   = StrLeft(HelpText,5);
    if (TmpStr == "Help:")
    { // Ok, a Help entry has been found
      TmpStr = StrRight(HelpText, StrGetLength(HelpText)-5);
      TmpStr = StrMakeLower(TmpStr);
      if (TmpStr == HelpLookup)
      { // THE Help entry has been found, show it to player
        Found = true;
        ShowHelp();
        HelpText = "End of Help";
      }
    }
  }
  CloseFile();
  if (Found)
  { // Return true so command processor will exit properly
    return true;
  }
  else
  { // Return false so command processor will tell player bad command
    return false;
  }
}

////////////////////////////////////////////////////////////
// Private functions static                               //
////////////////////////////////////////////////////////////

/***********************************************************
* Close Help file                                          *
************************************************************/

void Help::CloseFile()
{
  HelpFile.close();
}

/***********************************************************
* Open Help file                                           *
************************************************************/

bool Help::OpenFile()
{
  string HelpFileName;
  
  HelpFileName =  HELP_DIR;
  HelpFileName += "Help.txt";
  HelpFile.open(HelpFileName);
  if (HelpFile.is_open())
  {
    return true;
  }
  else
  {
    return false;
  }
}

/***********************************************************
* Read a line from Help file                               *
************************************************************/

void Help::ReadLine()
{
  string Buffer;

  getline (HelpFile, Buffer);
  HelpText = Buffer;
}

/***********************************************************
* Show help to player                                      *
************************************************************/

void Help::ShowHelp()
{
  TmpStr = StrLeft(HelpText, 13);
  while (TmpStr != "Related help:")
  {
    ReadLine();
    pDnodeActor->PlayerOut += HelpText;
    pDnodeActor->PlayerOut += "\r\n";
    TmpStr = StrLeft(HelpText, 13);
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}