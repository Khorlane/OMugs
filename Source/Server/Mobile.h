/***********************************************************
* OMugs - Online Multi-User Game Server                    *
* File:   Mobile.h                                         *
* Usage:  Define Mobile class                              *
* Author: Stephen L Bryant                                 *
*                                                          *
* This program belongs to Stephen L Bryant.                *
* It is considered a trade secret and is not to be         *
* divulged or used by parties who have not received        *
* written authorization from the owner.                    *
************************************************************/

#ifndef MOBILE_H
#define MOBILE_H

/***********************************************************
* Includes                                                 *
************************************************************/

#include "Config.h"
#include "Communication.h"
#include "Dnode.h"
#include "Utility.h"

/***********************************************************
* Define Mobile class                                      *
************************************************************/

class Mobile  
{
// Public functions static
  public:
    static  void    AddMobToRoom(CString RoomId, CString MobileId);
    static  int     CountMob(CString MobileId);
    static  int     CountMobOldWayNotUsedNow(CString MobileIdSearch);
    static  void    CreateMobPlayer(CString PlayerName, CString MobileId);
    static  void    CreateMobStatsFileWrite(CString Directory, CString MobileIdForFight, CString Stuff);
    static  void    CreatePlayerMob(CString PlayerName, CString MobileId);
    static  void    DeleteMobPlayer(CString PlayerName, CString MobileId);
    static  void    DeleteMobStats(CString MobileId);
    static  void    DeletePlayerMob(CString PlayerName);
    static  CString GetMobDesc1(CString MobileId);
    static  bool    IsMobileIdInRoom(CString RoomId, CString MobileId);
    static  Mobile *IsMobInRoom(CString MobileName);
    static  Mobile *IsMobValid(CString MobileId);
    static  CString MobAttacks(Mobile *pMobile);
    static  void    PutMobBackInRoom(CString PlayerName, CString RoomId);
    static  void    RemoveMobFromRoom(CString RoomId, CString MobileId);
    static  void    ShowMobsInRoom(Dnode *pDnode);
    static  void    UpdateMobInWorld(CString MobileId, CString AddRemove);
    static  void    WhereMob(CString MobileIdSearch);

// Public functions
  public:
    Mobile(CString MobileId);
    ~Mobile();
    void            CreateMobStatsFile(CString RoomId);
    void            ExamineMob(CString MobileId);
    CString         MobTalk();
    void            GetNextMobNbr();

// Private functions
  private:
    void            CloseFile();
    void            OpenFile(CString MobileId);
    void            ParseStuff();
    void            ReadLine();

// Public variables
  public:
    CString         Action;
    int             Armor;
    CString         Attack;
    int             Damage;
    CString         Desc1;
    CString         Desc2;
    CString         Desc3;
    int             ExpPoints;
    CString         Faction;
    int             HitPoints;
    bool            Hurt;
    int             Level;
    CString         Loot;
    CStdioFile      MobileFile;
    CString         MobileId;
    CString         MobNbr;
    CString         Names;
    CString         Stuff;
    CString         Talk;
};

#endif