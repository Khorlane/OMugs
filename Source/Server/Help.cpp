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

#include "stdafx.h"         // precompiled headers
#include "Help.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern Dnode   *pDnodeActor;
extern CString  CmdStr;

CStdioFile HelpFile;
CString    HelpText;

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
  CString HelpLookup;
  CString TmpStr;

  Found = false;
  if (!OpenFile())
  { // If the file isn't there, then all Helps are not found, doh!
    return false;
  }
  HelpLookup = Utility::GetWord(CmdStr, 2);
  HelpLookup.MakeLower();
  HelpText = "Not Done";
  while (HelpText != "End of Help")
  { // Loop until Help is found or end of file
    ReadLine();
    HelpText.TrimLeft();
    HelpText.TrimLeft();
    if (HelpText.Left(5) == "Help:")
    { // Ok, a Help entry has been found
      TmpStr = HelpText.Right(HelpText.GetLength() - 5);
      TmpStr.MakeLower();
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
  HelpFile.Close();
}

/***********************************************************
* Open Help file                                           *
************************************************************/

bool Help::OpenFile()
{
  CString HelpFileName;
  int     Success;
  
  HelpFileName =  HELP_DIR;
  HelpFileName += "Help.txt";
  Success = HelpFile.Open(HelpFileName,
            CFile::modeRead |
            CFile::typeText);
  if(Success)
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
  HelpFile.ReadString(HelpText);
}

/***********************************************************
* Show help to player                                      *
************************************************************/

void Help::ShowHelp()
{
  while (HelpText.Left(13 )!= "Related help:")
  {
    ReadLine();
    pDnodeActor->PlayerOut += HelpText;
    pDnodeActor->PlayerOut += "\r\n";
  }
  pDnodeActor->PlayerOut += "\r\n";
  pDnodeActor->pPlayer->CreatePrompt();
  pDnodeActor->PlayerOut += pDnodeActor->pPlayer->GetOutput();
}