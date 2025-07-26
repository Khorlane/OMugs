/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Help.h                                           *
* Usage:  Define Help class                                *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef HELP_H
#define HELP_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Dnode.h"
#include "Player.h"

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