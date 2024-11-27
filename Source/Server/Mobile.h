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
    static  void    AddMobToRoom(string RoomId, string MobileId);
    static  int     CountMob(string MobileId);
    static  void    CreateMobPlayer(string PlayerName, string MobileId);
    static  void    CreateMobStatsFileWrite(string Directory, string MobileIdForFight, string Stuff);
    static  void    CreatePlayerMob(string PlayerName, string MobileId);
    static  void    DeleteMobPlayer(string PlayerName, string MobileId);
    static  void    DeleteMobStats(string MobileId);
    static  void    DeletePlayerMob(string PlayerName);
    static  string  GetMobDesc1(string MobileId);
    static  bool    IsMobileIdInRoom(string RoomId, string MobileId);
    static  Mobile *IsMobInRoom(string MobileName);
    static  Mobile *IsMobValid(string MobileId);
    static  string  MobAttacks(Mobile *pMobile);
    static  void    PutMobBackInRoom(string PlayerName, string RoomId);
    static  void    RemoveMobFromRoom(string RoomId, string MobileId);
    static  void    ShowMobsInRoom(Dnode *pDnode);
    static  void    UpdateMobInWorld(string MobileId, string AddRemove);
    static  void    WhereMob(string MobileIdSearch);

// Public functions
  public:
    Mobile(string MobileId);
    ~Mobile();
    void            CreateMobStatsFile(string RoomId);
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