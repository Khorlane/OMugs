/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Dnode.h                                          *
* Usage:  Define Dnode class                               *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef DNODE_H
#define DNODE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "stdafx.h"

/***********************************************************
* Define Dnode class                                       *
************************************************************/

class Dnode
{

// Friend classes
  friend class Communication;
  friend class Descriptor;
  friend class Help;
  friend class Player;
  friend class Social;
  friend class Room;

// Protected functions static
  protected:
    static  int   GetCount();

// Protected functions
  protected:
    Dnode(int);
    ~Dnode();

// Protected variables static
  protected:
    static  int   Count;

// Protected variables
  protected:
    Dnode         *pDnodeNext;
    Dnode         *pDnodePrev;
    Player        *pPlayer;
    int           DnodeFd;
    CString       PlayerInp;
    CString       PlayerName;
    CString       PlayerNewCharacter;
    CString       PlayerOut;
    CString       PlayerPassword;
    bool          PlayerStateBye;
    bool          PlayerStateLoggingOn;
    bool          PlayerStateLoggedOn;
    bool          PlayerStatePlaying;
    bool          PlayerStateSendBanner;
    bool          PlayerStateWaitMaleFemale;
    bool          PlayerStateWaitName;
    bool          PlayerStateWaitNameConfirmation;
    bool          PlayerStateWaitNewCharacter;
    bool          PlayerStateWaitPassword;
    int           PlayerWrongPasswordCount;
};

#endif