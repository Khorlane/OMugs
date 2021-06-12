/***********************************************************
* H o l y Q u e s t  -  an attempt at something different  *
* File:   PlayerList.h                                     *
* Usage:  Define PlayerList class                          *
* Author: Steve Bryant (khorlane@www.holyquest.org)        *
*                                                          *
* This program belongs to Stephen L. Bryant.               *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef PLAYERLIST_H
#define PLAYERLIST_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Player.h"

/***********************************************************
* Define PlayerList class                                  *
************************************************************/

class PlayerList
{

// Public functions
  public:
    PlayerList();
    ~PlayerList();
    Player        *GetPlayerPtr() {return pPlayerCursor;};
    void          Append(Player *);
    void          DeletePlayer();
    bool          EndOfPlayerList();
    void          SetpPlayerCursorFirst();
    void          SetpPlayerCursorNext();
    void          SetpPlayerCursorPrev();

// Protected variables
  protected:
    Player        *pPlayerCursor;
    Player        *pPlayerHead;
    Player        *pPlayerTail;
};

#endif