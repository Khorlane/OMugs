/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   PlayerList.cpp                                   *
* Usage:  Manages a list of Players                        *
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
#include "PlayerList.h"
#include "Player.h"

/***********************************************************
* Constructor                                              *
************************************************************/

PlayerList::PlayerList()
{
  pPlayerHead   = 0;
  pPlayerTail   = 0;
  pPlayerCursor = 0;
}

/***********************************************************
* Destructor                                               *
************************************************************/

PlayerList::~PlayerList()
{
}

/***********************************************************
* Set cursor to next player in the list                    *
************************************************************/

void PlayerList::SetpPlayerCursorNext()
{
  pPlayerCursor = pPlayerCursor->pPlayerNext;
}

/***********************************************************
* Set cursor to previous player in the list                *
************************************************************/

void PlayerList::SetpPlayerCursorPrev()
{
  pPlayerCursor = pPlayerCursor->pPlayerPrev;
}

/***********************************************************
* Set cursor to first player in the list                   *
************************************************************/

void PlayerList::SetpPlayerCursorFirst()
{
  pPlayerCursor = pPlayerHead;
}

/***********************************************************
* Detect end of player list                                *
************************************************************/

bool PlayerList::EndOfPlayerList()
{
  if (pPlayerCursor == 0)
    return true;
  else
    return false;
}

/***********************************************************
* Append a player to the list                              *
************************************************************/

void PlayerList::Append(Player *pPlayer)
{
  if (pPlayerHead == 0) // List is empty
  {
    pPlayerHead           = pPlayer;
    pPlayerTail           = pPlayer;
    pPlayerCursor         = pPlayer;
    pPlayer->pPlayerNext  = 0;
    pPlayer->pPlayerPrev  = 0;
  }
  else // List is not empty
  {
    pPlayer->pPlayerPrev     = pPlayerTail;
    pPlayer->pPlayerNext     = 0;
    pPlayerTail->pPlayerNext = pPlayer;
    pPlayerTail              = pPlayer;
  }
}

/***********************************************************
* Delete a player from the list                            *
************************************************************/

void PlayerList::DeletePlayer()
{
  Player *pPlayerTmp;

  if (pPlayerCursor == pPlayerHead && pPlayerCursor == pPlayerTail)
  { // Only one player in the list
    pPlayerHead = 0;
    pPlayerTail = 0;
    pPlayerTmp  = 0;
  }
  else
  if (pPlayerCursor == pPlayerHead)
  { // Delete player at the head of the list
    pPlayerCursor->pPlayerNext->pPlayerPrev = 0;
    pPlayerHead                             = pPlayerHead->pPlayerNext;
    pPlayerTmp                              = pPlayerHead;
  }
  else
  if (pPlayerCursor == pPlayerTail)
  { // Delete player at the tail of the list
    pPlayerCursor->pPlayerPrev->pPlayerNext  = 0;
    pPlayerTail                       = pPlayerTail->pPlayerPrev;
    pPlayerTmp                        = pPlayerTail;
  }
  else
  { // well ... not one of the above condidtions
    pPlayerCursor->pPlayerPrev->pPlayerNext = pPlayerCursor->pPlayerNext;
    pPlayerCursor->pPlayerNext->pPlayerPrev = pPlayerCursor->pPlayerPrev;
    pPlayerTmp                              = pPlayerCursor->pPlayerPrev;
  }
  delete pPlayerCursor;
  pPlayerCursor = pPlayerTmp;
}