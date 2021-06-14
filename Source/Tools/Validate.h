/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Validate.h                                       *
* Usage:  Validate Rooms, Objects, Mobiles, etc            *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef VALIDATE_H 
#define VALIDATE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Log.h"
#include "Utility.h"

/***********************************************************
* Define Validate class                                    *
************************************************************/

class Validate
{

// Public functions
  public:
    Validate();
    ~Validate();
    void    static  LogValErr(CString Message, CString FileName);
    bool    static  ValidateIt(CString ValidationType);
    void    static  ValidateAll();
    void    static  ValidateLibraryLoot();
    void    static  ValidateLibraryMobiles();
    void    static  ValidateLibraryObjects();
    void    static  ValidateLibraryRooms();
    void    static  ValidateLibraryShops();
    void    static  ValidateLibraryWorldMobiles();
    void    static  ValidateRunningPlayers();
    void    static  ValidateRunningPlayersPlayerEqu();
    void    static  ValidateRunningPlayersPlayerObj();
    void    static  ValidateRunningRoomMob();
    void    static  ValidateRunningRoomObj();
};
#endif