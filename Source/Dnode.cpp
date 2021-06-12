/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   Dnode.cpp                                        *
* Usage:  Connection data                                  *
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
#include "Dnode.h"

/***********************************************************
* Global                                                   *
************************************************************/

int Dnode::Count = 0;
int Dnode::GetCount() {return Count;}

/************************************************************
 * Dnode constructor                                        *
 ************************************************************/

Dnode::Dnode(int SocketHandle)
{
  Count++;
  DnodeFd                             = SocketHandle;
  pPlayer                             = NULL;
  PlayerInp                           = "";
  PlayerName                          = "";
  PlayerNewCharacter                  = "";
  PlayerOut                           = "";
  PlayerPassword                      = "";
  PlayerWrongPasswordCount            = 0;

  PlayerStateBye                      = false;
  PlayerStateLoggingOn                = false;
  PlayerStateLoggedOn                 = false;
  PlayerStatePlaying                  = false;
  PlayerStateSendBanner               = true;
  PlayerStateWaitMaleFemale           = false;
  PlayerStateWaitName                 = false;
  PlayerStateWaitNameConfirmation     = false;
  PlayerStateWaitNewCharacter         = false;
  PlayerStateWaitPassword             = false;
}

/************************************************************
 * Dnode destructor                                         *
 ************************************************************/

Dnode::~Dnode()
{
  Count--;
  pDnodePrev->pDnodeNext = pDnodeNext;
  pDnodeNext->pDnodePrev = pDnodePrev;
}