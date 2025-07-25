/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Dnode.cpp                                        *
* Usage:  Connection data                                  *
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

#include "Dnode.h"

/***********************************************************
* Globals                                                  *
************************************************************/

int Dnode::Count = 0;

/***********************************************************
 * Dnode constructor                                       *
 ***********************************************************/

Dnode::Dnode(int SocketHandle, string IpAddress)
{
  Count++;
  DnodeFd                             = SocketHandle;
  FightTick                           = 0;
  HungerThirstTick                    = 0;
  InputTick                           = 0;
  pPlayer                             = NULL;
  PlayerInp                           = "";
  PlayerIpAddress                     = IpAddress;
  PlayerName                          = "Player name unknown";
  PlayerNewCharacter                  = "";
  PlayerOut                           = "";
  PlayerPassword                      = "";
  PlayerWrongPasswordCount            = 0;
  StatsTick                           = 0;

  PlayerStateAfk                      = false;
  PlayerStateBye                      = false;
  PlayerStateFighting                 = false;
  PlayerStateInvisible                = false;
  PlayerStateLoggingOn                = false;
  PlayerStatePlaying                  = false;
  PlayerStateSendBanner               = true;
  PlayerStateWaitMaleFemale           = false;
  PlayerStateWaitName                 = false;
  PlayerStateWaitNameConfirmation     = false;
  PlayerStateWaitNewCharacter         = false;
  PlayerStateWaitPassword             = false;
}

/***********************************************************
 * Dnode destructor                                        *
 ***********************************************************/

Dnode::~Dnode()
{
  Count--;
  pDnodePrev->pDnodeNext = pDnodeNext;
  pDnodeNext->pDnodePrev = pDnodePrev;
}

////////////////////////////////////////////////////////////
// Protected functions static                             //
////////////////////////////////////////////////////////////

/***********************************************************
* Return Dnode count                                       *
************************************************************/

int Dnode::GetCount()
{
  return Count;
}