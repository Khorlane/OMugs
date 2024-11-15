/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   World.h                                          *
* Usage:  Functions to make the world come alive.          *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef WORLD_H
#define WORLD_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Mobile.h"
//#include "Room.h"
// Osi
#include "Executor.h"
#include "Parser.h"

/***********************************************************
* Define World class                                       *
************************************************************/

class World
{

// Public functions static
  public:
    static  void    MakeMobilesMove();
    static  void    MakeMobilesMove1();
    static  void    MakeMobilesMove2();
    static  void    MakeMobilesMove3();
    static  void    CreateSpawnMobileEvents();
    static  void    CheckSpawnMobileEvents();
    static  void    Events();
    static  void    HealMobiles();
    static  bool    HealMobilesFightCheck(string Dir, string MobileId);
    static  void    Osi(string ScriptType, string ScriptId);
    static  void    SpawnMobile(CString MobileId, CString RoomId);
    static  void    SpawnMobileNoMove(CString MobileId);
};

#endif