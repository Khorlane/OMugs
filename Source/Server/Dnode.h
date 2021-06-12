/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Dnode.h                                          *
* Usage:  Define Dnode class                               *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef DNODE_H
#define DNODE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"

/***********************************************************
* Define Dnode class                                       *
************************************************************/

class Dnode
{

// Friend classes
  friend class Communication;
  friend class Descriptor;
  friend class Help;
  friend class Mobile;
  friend class Object;
  friend class Player;
  friend class Shop;
  friend class Social;
  friend class Room;
  friend class Utility;

// Public functions static
  public:
    static  int     GetCount();

// Protected functions
  protected:
    Dnode(int SocketHandle, CString IpAddress);
    ~Dnode();

// Protected variables static
  protected:
    static  int     Count;

// Protected variables
  protected:
    Dnode          *pDnodeNext;
    Dnode          *pDnodePrev;
    Player         *pPlayer;
    CString         CmdName1;
    CString         CmdName2;
    CString         CmdName3;
    clock_t         CmdTime1;
    clock_t         CmdTime2;
    clock_t         CmdTime3;
    int             DnodeFd;
    int             FightTick;
    int             HungerThirstTick;
    int             InputTick;
    CString         PlayerInp;
    CString         PlayerIpAddress;
    CString         PlayerName;
    CString         PlayerNewCharacter;
    CString         PlayerOut;
    CString         PlayerPassword;
    bool            PlayerStateAfk;
    bool            PlayerStateBye;
    bool            PlayerStateFighting;
    bool            PlayerStateInvisible;
    bool            PlayerStateLoggingOn;
    bool            PlayerStatePlaying;
    bool            PlayerStateReconnecting;
    bool            PlayerStateSendBanner;
    bool            PlayerStateWaitMaleFemale;
    bool            PlayerStateWaitName;
    bool            PlayerStateWaitNameConfirmation;
    bool            PlayerStateWaitNewCharacter;
    bool            PlayerStateWaitPassword;
    int             PlayerWrongPasswordCount;
    int             StatsTick;
};

#endif