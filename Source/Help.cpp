/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Help.cpp                                         *
* Usage:  Handles all help commands                        *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Player.h"
#include "Help.h"

/***********************************************************
* Globals                                                  *
************************************************************/

extern CString        GetWord(CString CmdStr, int WordNbr);
extern CString        GetWords(CString CmdStr, int WordNbr);
extern Communication  *pComm;

/***********************************************************
* Log constructor                                          *
************************************************************/

Help::Help()
{
}

/***********************************************************
* Log destructor                                           *
************************************************************/

Help::~Help()
{
}

/***********************************************************
* Is it a Help topic?                                      *
************************************************************/

bool Help::IsHelp(Dnode *pDnode1, CString CmdStr)
{
  bool    Found;
  CString HelpLookup;
  CString TmpStr;

  Found = false;
  if (!OpenFile())
  { // If the file isn't there, then all Helps are not found, doh!
    return false;
  }
  HelpLookup = GetWord(CmdStr, 2);
  HelpLookup.MakeLower();
  Stuff = "Not Done";
  while (Stuff != "End of Help")
  { // Loop until Help is found or end of file
    ReadLine();
    Stuff.TrimLeft();
    Stuff.TrimLeft();
    if (Stuff.Left(5) == "Help:")
    { // Ok, a Help has been found
      TmpStr = Stuff.Right(Stuff.GetLength() - 5);
      TmpStr.MakeLower();
      if (TmpStr == HelpLookup)
      { // THE Help has been found, show it to player
        Found = true;
        DoShowHelp(pDnode1, CmdStr);
        Stuff = "End of Help";
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

/***********************************************************
* Open Help file                                           *
************************************************************/

bool Help::OpenFile()
{
  CString     FileName;
  int         Success;
  
  FileName =  ".\\Library\\";
  FileName += "Help.txt";
  Success = HelpFile.Open(FileName,
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
* Close Help file                                          *
************************************************************/

void Help::CloseFile()
{
  HelpFile.Close();
}

/***********************************************************
* Read a line from Help file                               *
************************************************************/

void Help::ReadLine()
{
  HelpFile.ReadString(Stuff);
}

/***********************************************************
* Help command                                             *
************************************************************/

void Help::DoShowHelp(Dnode *pDnode1, CString CmdStr)
{
  while (Stuff.Left(13 )!= "Related help:")
  {
    ReadLine();
    pDnode1->PlayerOut += Stuff;
    pDnode1->PlayerOut += "\r\n";
  }
  pDnode1->PlayerOut += "\r\n";
  pDnode1->pPlayer->CreatePrompt();
  pDnode1->PlayerOut += pDnode1->pPlayer->GetOutput();
}