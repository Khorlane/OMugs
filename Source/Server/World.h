/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   World.h                                          *
* Usage:  Functions to make the world come alive.          *
* Author: Steve Bryant                                     *
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
    static  void    SpawnMobile(string MobileId, string RoomId);
    static  void    SpawnMobileNoMove(string MobileId);
};

#endif