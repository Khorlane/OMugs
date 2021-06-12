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

#include "Communication.h"

#include "Sqlite3.h"

/***********************************************************
* Define Room class                                        *
************************************************************/

class Room
{

// Public functions static
  public:
    Room();
    ~Room();
    void    static  DoSqlStatement(CString SqlStatement);
    int     static  FetchRows(void *NotUsed, int ColCount, char **ColValue, char **ColName);
    CString static  GetRoomId(CString RoomId);
    CString static  GetRoomName(CString RoomId);
    CString static  GetValidMobRoomExits(CString RoomId);
    bool    static  IsExit(CString MudCmdIsExit);
    bool    static  IsRoom(CString RoomId);
    bool    static  IsRoomType(CString RoomId, CString RoomType);
    void    static  ShowRoom(Dnode *pDnode);

// Private functions static
  private:
    void    static  CloseFile();
    void    static  MoveFollowers(Dnode *pDnode, CString ExitToRoomId);
    void    static  MovePlayer(Dnode *pDnode, CString ExitToRoomId);
    bool    static  OpenFile(Dnode *pDnode);
    void    static  ShowRoomDesc(Dnode *pDnode);
    void    static  ShowRoomExitDesc();
    void    static  ShowRoomExits(Dnode *pDnode);
    void    static  ShowRoomName(Dnode *pDnode);
};

#endif