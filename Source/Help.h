/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Help.h                                           *
* Usage:  Define Help class                                *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef HELP_H
#define HELP_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"
#include "Dnode.h"

/***********************************************************
* Define Help class                                        *
************************************************************/

class Help
{

// Public functions
  public:
    Help();
    ~Help();
    bool          IsHelp(Dnode *pDnode1, CString CmdStr);

// Private functions
  private:
    void          CloseFile();
    void          DoShowHelp(Dnode *pDnode1, CString CmdStr);
    bool          OpenFile();
    void          ReadLine();
    void          SendToPlayer(Dnode *pDnode1);

// Private variables
  private:
    CStdioFile    HelpFile;
    CString       Stuff;
};

#endif