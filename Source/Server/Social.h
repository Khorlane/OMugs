/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Social.h                                         *
* Usage:  Define Social class                              *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef SOCIAL_H
#define SOCIAL_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Communication.h"
#include "Dnode.h"
#include "Utility.h"

/***********************************************************
* Define Social class                                      *
************************************************************/

class Social
{

// Public functions
  public:
    Social();
    ~Social();
    bool    static  IsSocial();

// Private functions
  private:
    void    static  CloseFile();
    bool    static  OpenFile();
    bool    static  PositionNotOk(Dnode *pDnode, string MinPos);
    int     static  PosNbr(string Position);
    string  static  ReadLine();
    void    static  SendToPlayer(string MsgText);
    void    static  SendToTarget(Dnode *pDnodeTgt, string MsgText);
    void    static  Socialize(string MinPos, string MsgText);
};

#endif