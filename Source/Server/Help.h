/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Help.h                                           *
* Usage:  Define Help class                                *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef HELP_H
#define HELP_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Dnode.h"
#include "Player.h"
#include "Utility.h"

/***********************************************************
* Define Help class                                        *
************************************************************/

class Help
{

// Public functions
  public:
    Help();
    ~Help();
    bool    static  IsHelp();

// Private functions
  private:
    void    static  CloseFile();
    bool    static  OpenFile();
    void    static  ReadLine();
    void    static  ShowHelp();
};

#endif