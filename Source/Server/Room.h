/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Room.h                                           *
* Usage:  Define Room class                                *
* Author: Steve Bryant                                     *
************************************************************/

#ifndef ROOM_H
#define ROOM_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Communication.h"
#include "Dnode.h"
#include "Mobile.h"
#include "Object.h"
#include "Player.h"
#include "Utility.h"
#include "World.h"

/***********************************************************
* Define Room class                                        *
************************************************************/

class Room
{

// Public functions static
  public:
    Room();
    ~Room();
    string  static  GetRoomId(string RoomId);
    string  static  GetRoomName(string RoomId);
    string  static  GetValidMobRoomExits(string RoomId);
    bool    static  IsExit(string MudCmdIsExit);
    bool    static  IsRoom(string RoomId);
    bool    static  IsRoomType(string RoomId, string RoomType);
    void    static  ShowRoom(Dnode *pDnode);

// Private functions static
  private:
    void    static  CloseFile();
    void    static  MoveFollowers(Dnode *pDnode, string ExitToRoomId);
    void    static  MovePlayer(Dnode *pDnode, string ExitToRoomId);
    bool    static  OpenFile(Dnode *pDnode);
    void    static  ShowRoomDesc(Dnode *pDnode);
    void    static  ShowRoomExitDesc();
    void    static  ShowRoomExits(Dnode *pDnode);
    void    static  ShowRoomName(Dnode *pDnode);
};

#endif