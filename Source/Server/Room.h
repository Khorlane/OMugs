/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Room.h                                           *
* Usage:  Define Room class                                *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
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
    int     static  FetchRows(void *NotUsed, int ColCount, char **ColValue, char **ColName);
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